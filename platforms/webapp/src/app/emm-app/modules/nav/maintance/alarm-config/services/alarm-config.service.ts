import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { AlarmConfigState } from '../models/alarm-static.model';
import { IConfigData, IOption } from '../domain';
import { TranslateService } from '@ngx-translate/core';
import { map } from 'rxjs/operators';
@Injectable({
    providedIn: 'root'
})
export class AlarmConfigService {

    constructor(private http: HttpService, private translate: TranslateService) {
    }
    public levelOption: IOption[] = [{
        id: 'Critical',
        label: this.translate.instant('ALARM_EVENT_CRITICAL'),
    }, {
        id: 'Major',
        label: this.translate.instant('ALARM_EVENT_MAJOR'),
    }, {
        id: 'Minor',
        label: this.translate.instant('ALARM_EVENT_MINOR'),
    }, {
        id: 'Normal',
        label: this.translate.instant('ALARM_EVENT_NORMAL'),
    }];

    // 获取级别配置
    public getAlarmConfigInfo() {
        const url = '/UI/Rest/Maintenance/Enclosure/RemovedEventSeverity';
        return this.http.get(url).pipe(
            map(
                (data) => {
                    return this.getConfigInfo(data['body']);
                }
            )
        );
    }
    private getConfigInfo(newData) {
        const srcData: IConfigData[] = [];
        if (newData) {
            Object.keys(newData).forEach((key) => {
                srcData.push(this.initializationInfo(key, newData[key]));
            });
        }
        return this._sort(srcData);
    }
    public saveAlarmConfigInfo(params) {
        return this.http.patch('/UI/Rest/Maintenance/Enclosure/RemovedEventSeverity', params);
    }
    // 排序
    private _sort(arr) {
        arr.sort((a, b) => {
            return b.sortId - a.sortId;
        });
        return arr;
    }
    public getSelectId(value: string) {
        let selectOption = {};
        this.levelOption.forEach((item) => {
            if (item.id === value) {
                selectOption = item;
            }
        });
        return selectOption;
    }
    // 数据处理
    public initializationInfo(key: string, item: string) {
        const alarmLevel = AlarmConfigState.getState(item);
        const levelAlarm: IConfigData = {
            label: '',
            id: '',
            sortId: 0,
            selectOption: this.getSelectId(item),
            options: this.levelOption,
            alarmLevel
        };
        switch (key) {
            case 'Blade':
                levelAlarm.label = this.translate.instant('EMM_ALARM_CONFIG_COMPUTE_NODE');
                levelAlarm.id = 'Blade';
                levelAlarm.sortId = 0;
                break;
            case 'Swi':
                levelAlarm.label = this.translate.instant('EMM_ALARM_CONFIG_DIRECTLY');
                levelAlarm.id = 'Swi';
                levelAlarm.sortId = 1;
                break;
            case 'SMM':
                levelAlarm.label = this.translate.instant('EMM_ALARM_CONFIG_MANAGE');
                levelAlarm.id = 'SMM';
                levelAlarm.sortId = 2;
                break;
            case 'PSU':
                levelAlarm.label = this.translate.instant('EMM_ALARM_CONFIG_POWER');
                levelAlarm.id = 'PSU';
                levelAlarm.sortId = 3;
                break;
            case 'Fan':
                levelAlarm.label = this.translate.instant('EMM_ALARM_CONFIG_FAN');
                levelAlarm.id = 'Fan';
                levelAlarm.sortId = 4;
                break;
            default:
                break;
        }
        return levelAlarm;
    }
}
