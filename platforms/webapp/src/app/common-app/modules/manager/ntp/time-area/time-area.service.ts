import { Injectable } from '@angular/core';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { NtpService } from '../ntp.service';
import { TimeAreaData, TimeArea, IZreo, IParam, NTPStaticModel } from '../models';
import { map } from 'rxjs/internal/operators/map';
import { mergeMap } from 'rxjs/internal/operators/mergeMap';
import { HttpService, UserInfoService, GlobalDataService } from 'src/app/common-app/service';
import { LanguageService } from './../../../../service/language.service';

@Injectable({
    providedIn: 'root'
})
export class TimeAreaService {

    public locale: string;

    constructor(
        private http: HttpService,
        private ntpService: NtpService,
        private user: UserInfoService,
        private global: GlobalDataService,
        private langService: LanguageService
    ) { }

	/**
	 * 地区时区工厂，返回一个观察者给组件，获取地区和时区生产过程
	 */
    public factory() {
        const post1 = this.getAreaList(this.locale);
        const post2 = this.getInitTime();
        const timeAreaData = new TimeAreaData();
        const timeArea = new TimeArea();
        const source1$ = forkJoin([post1, post2]).pipe(
            map(([postData1, postData2]) => {
                const area = postData2.time[1] ? postData2.time[1] : postData1[0].label;
                const timeZoneConfig = postData2.timeZoneConfig;
                const privilege = this.user.hasPrivileges(NTPStaticModel.getInstance().privileges);
                timeAreaData.setTimeDisable(privilege, timeZoneConfig);
                timeArea.setTime = postData2.time[0] || '';
                timeArea.setArea = area;
                timeAreaData.setAreaList = postData1;
                timeAreaData.setIbmcTime = postData2.ibmcTime;
                timeAreaData.setAreaSelect = this.getAreaSelect(postData1, area);
                timeAreaData.setDstEnabled = postData2.dstEnabled;
                this.ntpService.timeNTP.setTimeArea = timeArea;
            }),
            mergeMap(() => {
                return this.getTimeList(timeArea.getArea, this.locale).pipe(
                    mergeMap((timeList) => {
                        timeAreaData.setTimeList = timeList;
                        return this.getTimeSelect(timeList, timeArea.getArea, timeArea.getTime).pipe(
                            map((timeData) => {
                                timeAreaData.setTimeOrigin = timeData.time;
                                timeAreaData.setTimeSelect = timeData.time;
                                timeAreaData.setParamList = timeData.paramList;
                                timeArea.setTime = timeData.time.key;
                                return { timeAreaData, timeArea };
                            })
                        );
                    })
                );
            })
        );
        return source1$;
    }

	/**
	 * 获取接口返回当前的地区和时区
	 */
    private getInitTime() {
        return this.ntpService.data.pipe(
            map((results) => {
                let time: [string, string];
                let check = results.DateTimeLocalOffset;
                const timeZoneConfig = results.TimeZoneConfigurable;
                const dstEnabled = results.DstEnabled;
                let ibmcTime = '(UTC' + results.TimeZone + ')';
                if (!check) {
                    return { time, timeZoneConfig, dstEnabled};
                } else if (/^UTC/.test(check) || /^GMT/.test(check)) {
                    check = check.replace('GMT', 'UTC');
                    time = [check, 'UTC'];
                } else {
                    const index = check.indexOf('/');
                    time = [check.substring(index + 1), check.slice(0, index)];
                }

                // 处理法语
                if (this.langService.locale === 'fr-FR') {
                    ibmcTime = ibmcTime.replace(/:/g, 'h');
                }

                // 页脚时间偏移量刷新
                this.global.ibmcTime.next({
                    offset: ibmcTime
                });
                return { time, timeZoneConfig, ibmcTime, dstEnabled};
            })
        );
    }

	/**
	 * 获取地区下拉列表
	 * @param locale 语言
	 */
    private getAreaList(locale: string) {
        const url = `assets/common-assets/i18n/time-area-data/area-data/${locale}.json`;
        return this.getData(url).pipe(
            map((results) => {
                return this.transform(results).sort(
                    (param1, param2) => {
                        return param1.label.localeCompare(param2.label);
                    }
                );
            })
        );
    }

	/**
	 * 获取时区下拉列表
	 * @param area 地区返回值
	 * @param locale 语言
	 */
    private getTimeList(area: string, locale: string) {
        const url = `assets/common-assets/i18n/time-area-data/time-data/${area}/${locale}.json`;
        return this.getData(url).pipe(
            map((val) => {
                const arr = area === 'UTC' ? this.transform(val) : this.transform(val).sort(
                    (param1, param2) => {
                        return param1.label.localeCompare(param2.label);
                    }
                );
                return arr;
            })
        );
    }

	/**
	 * 获取地区下拉列表选中项
	 * @param areaList 地区列表
	 * @param area 地区返回值
	 */
    private getAreaSelect(areaList: IZreo[], area: string): IZreo {
        const izero: IZreo[] = areaList.filter((val) => val.key === area);
        if (!izero || izero.length === 0) {
            return areaList[0];
        }
        return izero[0];
    }

	/**
	 * 获取当前时区下拉列表和下拉列表选中项
	 * @param timeList 时区列表
	 * @param area 地区返回值
	 * @param time 时区返回值
	 */
    private getTimeSelect(timeList: IZreo[], area: string, time: string) {
        const fileName = area + '.json';
        const url = `assets/common-assets/i18n/time-area-data/time-data/${area}/${fileName}`;
        return this.getData(url).pipe(
            map((list: any) => {
                const paramList: IParam[] = list;
                let select: IParam = null;
                if (time.indexOf('UTC') > -1) {
                    select = paramList.filter((obj: any) => obj.value === time || obj.content === time)[0];
                } else {
                    select = paramList.filter((obj: any) => obj.value.indexOf(time) > -1 || obj.content.indexOf(time) > -1)[0];
                }
                const izero = select ? timeList.filter((val) => val.key === select.content) : [];
                if (izero.length > 0) {
                    return { time: izero[0], paramList };
                } else {
                    return { time: timeList[0], paramList };
                }
            })
        );
    }

    public areaChange(area: string, locale: string, timeOrigin: IZreo) {
        return this.getTimeList(area, locale).pipe(
            mergeMap(
                (timeList) => {
                    return this.getTimeSelect(timeList, area, timeOrigin.key).pipe(
                        map((timeData) => {
                            return { timeSelect: timeData.time, timeList, paramList: timeData.paramList };
                        })
                    );
                }
            )
        );
    }

    public saveTimeArea(ntpCopy: any, ntpTimeArea: TimeArea, paramList: IParam[]): object {
        const obj = {DateTimeLocalOffset: ''};
        if (JSON.stringify(ntpCopy) !== JSON.stringify(ntpTimeArea)) {
            // 重新组装下发数据
            const time  = ntpTimeArea['time'];
            const select = paramList.filter((item) => item.value === time || item.content === time)[0];
            obj.DateTimeLocalOffset = select.value;
        } else {
           delete obj.DateTimeLocalOffset;
        }
        return obj;
    }

    public saveDstEnabled(dstEnabledCopy: boolean, dstEnabledFinal: boolean): object {
        const obj = {DstEnabled: true};
        if (dstEnabledCopy !== dstEnabledFinal) {
            // 重新组装下发数据
            obj.DstEnabled = dstEnabledFinal;
        } else {
           delete obj.DstEnabled;
        }
        return obj;
    }

	/**
	 * 发送请求
	 * @param url 请求链接
	 */
    private getData(url: string) {
        return this.http.get(url).pipe(
            map(data => data.body)
        );
    }

	/**
	 * 将json数据转化为需要的结构体
	 * @param obj 接口返回值
	 */
    private transform(obj: {}): IZreo[] {
        const arr: IZreo[] = [];
        for (const key in obj) {
            if (obj[key] === 0 || obj[key]) {
                arr.push({
                    key,
                    label: obj[key]
                });
            }
        }
        return arr;
    }

}
