import { IZreo, IParam } from '..';

export class TimeAreaData {
    // 时区
    private timeList: IZreo[];
    // 地区
    private areaList: IZreo[];

    private paramList: IParam[];

    private timeSelect: IZreo;

    private areaSelect: IZreo;

    private ibmcTime: string;

    private timeDiable: boolean;
    // 接口返回的时区选中项
    private timeOrigin: IZreo;
    // 夏令时开关状态
    private dstEnabled: boolean;
    set setTimeOrigin(timeOrigin: IZreo) {
        this.timeOrigin = timeOrigin;
    }

    get getTimeOrigin(): IZreo {
        return this.timeOrigin;
    }

    set setAreaList(areaList: IZreo[]) {
        this.areaList = areaList;
    }

    get getAreaList(): IZreo[] {
        return this.areaList;
    }

    set setTimeList(timeList: IZreo[]) {
        this.timeList = timeList;
    }

    get getTimeList(): IZreo[] {
        return this.timeList;
    }

    /**
     * 时区选中项模型设置方法
     * 1、存在时区偏移量 ibmcTime
     * 2、UTC时区不添加偏移量
     * 3、已经添加偏移量的时区选中项不重复添加
     * 4、只需要给接口返回的时区选中项添加偏移量
     */
    set setTimeSelect(timeSelect: IZreo) {
        if (this.ibmcTime &&
            timeSelect.label.indexOf('UTC') < 0 &&
            timeSelect.label.indexOf(this.ibmcTime) < 0 &&
            timeSelect.key === this.timeOrigin.key
        ) {
            timeSelect.label = timeSelect.label + this.ibmcTime;
        }
        this.timeSelect = timeSelect;
    }

    get getTimeSelect(): IZreo {
        return this.timeSelect;
    }

    set setAreaSelect(areaSelect: IZreo) {
        this.areaSelect = areaSelect;
    }

    get getAreaSelect(): IZreo {
        return this.areaSelect;
    }

    set setParamList(paramList: IParam[]) {
        this.paramList = paramList;
    }

    get getParamList(): IParam[] {
        return this.paramList;
    }

    set setIbmcTime(ibmcTime: string) {
        if (this.timeSelect && this.timeSelect.label.indexOf(this.ibmcTime) > -1) {
            this.timeSelect.label = this.timeSelect.label.replace(this.ibmcTime, ibmcTime);
        } else if (this.timeSelect && this.timeSelect.label.indexOf('UTC') < 0) {
            this.timeSelect.label = this.timeSelect.label + ibmcTime;
        }
        this.ibmcTime = ibmcTime;
    }

    /**
     * 时区和地区下拉框是否可编辑
     * @param privilege 用户权限
     * @param timeZoneConfig 环境是否支持设置
     * @param systemLock 系统锁定
     */
    public setTimeDisable(
        privilege: boolean,
        timeZoneConfig: boolean
    ) {
        this.timeDiable = !privilege || !timeZoneConfig;
    }

    get getTimeDisable(): boolean {
        return this.timeDiable;
    }

    set setDstEnabled(dstEnabled: boolean) {
        this.dstEnabled = dstEnabled;
    }

    get getDstEnabled(): boolean {
        return this.dstEnabled;
    }
}

