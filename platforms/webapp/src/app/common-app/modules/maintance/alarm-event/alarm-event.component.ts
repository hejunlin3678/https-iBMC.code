import { Component, OnInit, ViewChild } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { AlarmEventService } from './services';
import { TiTableRowData, TiTableColumns, TiButtonItem, TiDateValue } from '@cloud/tiny3';
import { ITabs } from './domain';
import Date from 'src/app/common-app/utils/date';
import { AlertMessageService, LoadingService, CommonService, UserInfoService, PRODUCT } from 'src/app/common-app/service';
import { FilterConditionComponent } from './component';
import { ProductService } from 'src/app/bmc-app/modules/nav/system/info/product/service';
import { CommonData } from 'src/app/common-app/models';
@Component({
    selector: 'app-alarm-event',
    templateUrl: './alarm-event.component.html',
    styleUrls: ['./alarm-event.component.scss'],
})
export class AlarmEventComponent implements OnInit {
    @ViewChild('filterCondition', {static: true}) filterConditionComponent: FilterConditionComponent;
    constructor(
        private service: AlarmEventService,
        private translate: TranslateService,
        private commonService: CommonService,
        private alert: AlertMessageService,
        private loading: LoadingService,
        private user: UserInfoService,
        private productService: ProductService,
    ) { }
    public isSystemLock: boolean = this.user.systemLockDownEnabled;
    public privileges = this.user.hasPrivileges(['ConfigureComponents']);
    public type = 'curent';

    public tabs: ITabs[] = [{
        id: 'curent',
        title: 'CURRENT_ALARM',
        active: true
    }, {
        id: 'system',
        title: 'SYSTEM_EVENT',
        active: false
    }];
    public exportUrl: string = '';
    public clearUrl: string = '';
    public systemLogUrl: string = '';
    public searchOnceFlag: boolean = false;
    // 当前事件
    public currentEvent: any = {
        displayed: [],
        srcData: {
            data: [],
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        }
    };
    // 系统事件
    public sysEvent: any = {
        displayed: [],
        srcData: {
            data: [],
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        }
    };
    // 定义筛选条件
    public conditionsTitle: TiButtonItem[] = [
        {
            id: 1,
            nameId: 'alarmLevel',
            title: 'ALARM_EVENT_ALARM_LEVEL',
            // 告警级别多选
            selected: [],
            items: [{
                id: null,
                text: this.translate.instant('ALARM_EVENT_ALL')
            }, {
                id: 'Critical',
                text: this.translate.instant('ALARM_EVENT_CRITICAL')
            }, {
                id: 'Major',
                text: this.translate.instant('ALARM_EVENT_MAJOR')
            }, {
                id: 'Minor',
                text: this.translate.instant('ALARM_EVENT_MINOR')
            }, {
                id: 'Informational',
                text: this.translate.instant('ALARM_EVENT_NORMAL')
            }]
        }, {
            id: 2,
            nameId: 'eventType',
            title: 'ALARM_EVENT_TYPE',
            // 主题类型多选
            selected: [],
            items: []
        }, {
            id: 3,
            nameId: 'generationTime',
            title: 'ALARM_EVENT_ALERT_TIME',
            selected: '',
            items: [{
                id: null,
                text: this.translate.instant('ALARM_EVENT_ALL'),
                isShow: true
            }, {
                id: 2,
                text: this.translate.instant('ALARM_EVENT_DATE1'),
                isShow: true
            }, {
                id: 3,
                text: this.translate.instant('ALARM_EVENT_DATE2'),
                isShow: true
            }, {
                id: 4,
                text: this.translate.instant('ALARM_EVENT_DATE3'),
                isShow: true
            }, {
                id: 5,
                text: this.translate.instant('ALARM_EVENT_DATE4'),
                isShow: false
            }]
        }
    ];
    public searchValue: string = '';
    public downloading: boolean = false;
    public searchPlacehoder: string = 'ALARM_EVENT_SEARCH_PALCEHOLDER';
    public format1: string = 'yyyy.MM.dd';
    public value1: TiDateValue = {
        begin: new Date(),
        end: new Date(new Date().setDate(new Date().getDate() + 7))
    };
    // 设置当前表格数据
    public srcData: object[] = this.currentEvent.srcData;
    // 定义表头
    public currentEventColumns: TiTableColumns[] = [
        {
            title: 'EVENT_ORDER',
            width: '10%'
        },
        {
            title: 'EVENT_LEVEL',
            width: '10%'
        },
        {
            title: 'EVENT_MAIN_TYPE',
            width: '10%'
        },
        {
            title: 'EVENT_CODE',
            width: '15%'
        },
        {
            title: 'EVENT_ENGENDER_TIME',
            width: '15%'
        },
        {
            title: 'EVENT_DESC',
            width: '40%'
        },
        {
            title: 'EVENT_SUGGESTION',
            width: '10%'
        }
    ];
    public sysEventColumns: TiTableColumns[] = [
        {
            title: 'EVENT_ORDER',
            width: '5%'
        },
        {
            title: 'EVENT_LEVEL',
            width: '10%'
        },
        {
            title: 'EVENT_MAIN_TYPE',
            width: '10%'
        },
        {
            title: 'EVENT_STATUS',
            width: '10%'
        },
        {
            title: 'EVENT_CODE',
            width: '10%'
        },
        {
            title: 'EVENT_ENGENDER_TIME',
            width: '15%'
        },
        {
            title: 'EVENT_DESC',
            width: '30%'
        },
        {
            title: 'EVENT_SUGGESTION',
            width: '10%'
        }
    ];
    // 设置当前表头
    public columns: TiTableColumns[] = this.currentEventColumns;
    public searchedData: TiTableRowData[] = [];

    // 分页
    public currentPage: number = 1;
    public totalNumber: number = 0;
    public pageSize: { options: number[], size: number; } = {
        options: [15, 30, 50],
        size: 15
    };
    // 表格数据
    public sysLogInfo: object[] = [];
    // 系统日志查询条件初始化
    private queryParams = {
        Skip: 1,
        Top: 15,
        Level: '',
        Subject: '',
        BeginTime: '',
        EndTime: '',
        SearchString: ''
    };
    // 主体类型对照表
    public objectTypeList: object[] = [];
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    ngOnInit() {
        this.getSubjectType();
        // 告警级别默认选中（多选）：全部
        this.conditionsTitle[0].selected = [this.conditionsTitle[0].items[0]];
        // 产生日期默认选中（单选）：全部
        this.conditionsTitle[2].selected = this.conditionsTitle[2].items[0];
        this.getInitData('current');
    }
    // 查询当前日志
    private _queryLogInfo(callback) {
        this.loading.state.next(true);
        this.service.logServices().subscribe((response) => {
            const res = response['body'];
            this.sysLogInfo = res.List;
            this.totalNumber = res.TotalCount;
            callback();
        }, (error) => {
            this.loading.state.next(false);
        });
    }

    // 查询系统日志
    public _querySystemLog(callback, par) {
        this._getParams(par);
        const params = this._deleteNULL(this.queryParams);
        this.loading.state.next(true);

        this.service.sysLogServices(params).subscribe((response) => {
            const res = response['body'];
            this.sysLogInfo = res.List;
            this.totalNumber = res.TotalCount;
            callback();
            this.loading.state.next(false);
        }, (error) => {
            this.loading.state.next(false);
        });
    }
    // 获取主体类型
    public getSubjectType() {
        this.service.getSubjectType().subscribe((subjectType) => {
            if (CommonData.productType !== PRODUCT.IRM) {
                this.productService.getData().subscribe((data) => {
                    if (data.boardId === '0x00ff') {
                        subjectType.forEach(item => {
                            if (item.ID !== 16) {
                                this.objectTypeList.push(item);
                            }
                        });
                    } else {
                        this.objectTypeList = subjectType;
                    }
                });
            } else if (CommonData.productType === PRODUCT.IRM) {
                this.objectTypeList = subjectType;
            }
        });
    }
    // tab页切换触发
    public tabChange(isActive: boolean, type: string): void {
        if (isActive && type === 'curent') {
            this.type = 'curent';
            this._setColumns(this.currentEventColumns);
            this.getInitData('current');
        } else if (isActive && type === 'system') {
            this.type = 'system';
            this._setColumns(this.sysEventColumns);
            this.getInitData('system');
        }
        this.currentPage = 1;
    }
    private _setColumns(columns: object[]) {
        this.columns = columns;
    }
    private _setPagination(currentPage: number = 1, pageSize: any = { options: [15, 30, 50], size: 15 }, totalNumber: number = 0) {
        this.currentPage = currentPage;
        this.totalNumber = totalNumber;
        this.pageSize = pageSize;
    }
    // 当操作选择框改变每页显示条数或操作页码改变当前页码时会触发该事件，向外通知当前页码、每页显示条数和总条数。
    public pageChange(pageParams) {
        this.currentPage = pageParams.currentPage;
        this.totalNumber = pageParams.totalNumber;
        // 切换分页设置查询参数（Level：告警级别）
        let levelIds = [];
        if (this.conditionsTitle[0].selected?.length > 0) {
            levelIds = (this.conditionsTitle[0].selected || []).map(e => e.id);
        }
        if (levelIds.length === 1 && levelIds[0] === null) {
            levelIds = [];
        }
        // 切换分页设置查询参数（Subject：主题类型）
        let subjectIds = [];
        if (this.conditionsTitle[1].selected?.length > 0) {
            subjectIds = (this.conditionsTitle[1].selected || []).map(e => e.id);
        }
        if (subjectIds.length === 1 && subjectIds[0] === null) {
            subjectIds = [];
        }
        const params = {
            Skip: this.currentPage,
            Top: this.pageSize.size,
            SearchString: this.searchValue,
            Level: levelIds,
            Subject: subjectIds,
            BeginTime: this.value1.begin,
            EndTime: this.value1.end
        };
        this.querySysLog(params);
    }
    // 初始化获取表格数据
    public getInitData(type: string) {
        if (type && type === 'current') {
            this.srcData['data'] = [];
            this._queryLogInfo(() => {
                const tempArr = [];
                this.sysLogInfo.forEach((item) => {
                    tempArr.push({
                        level: item['Level'],
                        subjectType: item['Subject'],
                        eventDetails: this._dealDetails(item['HandlingSuggestion']),
                        eventDescription: item['Content'],
                        eventCode: item['EventCode'],
                        isDetails: this._hasDetails(item['HandlingSuggestion']),
                        createTime: this._dealDate(item['CreatedTime'])
                    });
                });
                this.currentEvent.srcData.data = this.sort(tempArr);
                this.srcData = this.currentEvent.srcData;
                this.loading.state.next(false);
            });
        } else if (type && type === 'system') {
            this.srcData['data'] = [];
            if (!this.searchOnceFlag) {
                this._setFilterCondation();
                this.searchOnceFlag = true;
            } else {
                this.filterConditionComponent.initData();
                this.resetSearch();
            }
            const params = {
                Skip: 1,
                Top: this.pageSize.size,
                SearchString: '',
                Level: '',
                Subject: '',
                BeginTime: '',
                EndTime: ''
            };
            this.querySysLog(params);
        }
        this._setPagination(this.currentPage, { options: [15, 30, 50], size: 15 }, this.totalNumber);
    }
    // query系统信息
    public querySysLog(params) {
        this._querySystemLog(() => {
            const tempArr = [];
            this.sysLogInfo.forEach((item) => {
                tempArr.push({
                    orderNumber: item['ID'],
                    level: item['Level'],
                    subjectType: item['SubjectName'],
                    eventDetails: this._dealDetails(item['HandlingSuggestion'], item['Status']),
                    eventDescription: item['Content'],
                    eventCode: item['EventCode'],
                    isDetails: this._hasDetails(item['HandlingSuggestion'], item['Status']),
                    createTime: item['CreatedTime'],
                    status: item['Status']
                });
            });
            this.sysEvent.srcData.data = tempArr;
            this.srcData = this.sysEvent.srcData;
        }, params);
    }
    // 清空日志
    public clearLog() {
        this.service.clearData().subscribe((response) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            const params = {
                Skip: 1,
                Top: 15,
                SearchString: this.searchValue,
                Level: this.conditionsTitle[0].selected.id,
                Subject: this.conditionsTitle[1].selected.id,
                BeginTime: this.value1.begin,
                EndTime: this.value1.end
            };
            this.querySysLog(params);
        }, () => {
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
        });
    }
    // 下载日志
    public downloadLog() {
        this.downloading = true;
        this.service.downloadLog().subscribe((response) => {
            const res = response['body'];
            this._getTaskState(res.url);
        }, (error) => {
            this.downloading = false;
        });
    }
    // 查询按钮
    public queryLog(params) {
        this._setSelect(params);
        const par = {
            Skip: this.currentPage,
            Top: this.pageSize.size,
            SearchString: params.SearchString,
            Level: params.Level,
            Subject: params.Subject
        };
        if (params.value1) {
            par['BeginTime'] = params.value1.begin;
            par['EndTime'] = params.value1.end;
        }
        this.querySysLog(par);
    }
    // 重置按钮
    public resetSearch() {
        this.searchValue = '';
        // 重置告警级别：全部
        this.conditionsTitle[0].selected = [this.conditionsTitle[0].items[0]];
        // 重置主题类型：全部
        this.conditionsTitle[1].selected = [this.conditionsTitle[1].items[0]];
        this.conditionsTitle[2].selected = this.conditionsTitle[2].items[0];
    }
    // 当前告警数据排序
    public sort(arr: object[]) {
        if (arr.length === 0) {
            return arr;
        }
        arr.sort((a, b) => {
            return (new Date(b['createTime']).getTime()) - (new Date(a['createTime']).getTime());
        });
        let count = arr.length;
        arr.forEach((item) => {
            item['orderNumber'] = count;
            count--;
        });
        return arr;
    }
    private _setSelect(params): void {
        // 告警级别回显选中项
        this._setLevelSelect(params);
        // 主题类型回显选中项
        this._setSubjectSelect(params);
        // 日志日期范围回显选中项
        this._setTimeSelectSelect(params);
    }
    /**
     * 告警级别回显选中项
     * @param params 筛选回显信息
     */
    private _setLevelSelect(params): void {
        this.conditionsTitle[0].selected = [];
        if (params.Level && params.Level.length > 0) {
            params.Level.forEach(element => {
                this.conditionsTitle[0].items.forEach((key) => {
                    if (key.id === element) {
                        this.conditionsTitle[0].selected.push(key);
                    }
                });
            });
        } else {
            this.conditionsTitle[0].selected = [this.conditionsTitle[0].items[0]];
        }
    }
    /**
     * 主题类型回显选中项
     * @param params 筛选回显信息
     */
    private _setSubjectSelect(params): void {
        this.conditionsTitle[1].selected = [];
        this.conditionsTitle[1].selected = [];
        if (params.Subject && params.Subject.length > 0) {
            params.Subject.forEach(element => {
                this.conditionsTitle[1].items.forEach((key) => {
                    if (key.id === element) {
                        this.conditionsTitle[1].selected.push(key);
                    }
                });
            });
        } else {
            this.conditionsTitle[1].selected = [this.conditionsTitle[1].items[0]];
        }
    }
    /**
     * 日志日期范围回显选中项
     * @param params 筛选回显信息
     */
    private _setTimeSelectSelect(params): void {
        this.conditionsTitle[2].items.forEach((key) => {
            if (key.id === params.timeSelect) {
                this.conditionsTitle[2].selected = key;
            }
        });
    }
    // 获取下载状态
    private _getTaskState(url) {
        const _self = this;
        this.service.getTaskStatus(url).subscribe((response) => {
            const taskData = response['body'];
            if (taskData.prepare_progress === 100 && taskData.downloadurl) {
                _self.commonService.restDownloadFile(taskData.downloadurl).then(() => {
                    this.downloading = false;
                }, () => {
                    this.downloading = false;
                });
            } else {
                _self._getTaskState(url);
            }
        }, (error) => {
            this.downloading = false;
        });

    }
    private _objKeySort(arr: object) {
        const newkey = Object.keys(arr).sort();
        const newObj = {};
        for (const key of newkey) {
            newObj[key] = arr[key];
        }
        return newObj;
    }
    // 设置过滤条件
    private _setFilterCondation() {
        const arr = this.objectTypeList;
        let tempArr = {};
        arr.forEach((item) => {
            tempArr[item['Label']] = item['Label'];
        });
        tempArr = this._objKeySort(tempArr);
        let index = 0;
        for (const key in tempArr) {
            if (Object.prototype.hasOwnProperty.call(tempArr, key)) {
                if (index === 0) {
                    this.conditionsTitle[1].items.push({
                        id: null,
                        text: this.translate.instant('ALARM_EVENT_ALL')
                    });
                }
                this.conditionsTitle[1].items.push({
                    id: key,
                    text: key
                });
                index++;
            }
        }
        // 主体类型默认选中：全部
        this.conditionsTitle[1].selected = [this.conditionsTitle[1].items[0]];
    }

    // 当前事件主体类型处理
    private _dealSubject(eventId) {
        const eventSubjectID = eventId.substring(2, 4);
        for (const key of this.objectTypeList) {
            if (key['ID'] === parseInt(eventSubjectID, 16)) {
                return key['Label'];
            }
        }
        return '';
    }
    // 系统事件主体类型处理
    private _dealSysSubject(subjectType: number | string) {
        for (const key of this.objectTypeList) {
            if (Number(key['ID']) === Number(subjectType)) {
                return key['Label'];
            }
        }
        return '';
    }
    // 处理日期时间
    private _dealDate(date: string) {
        if (date === '') {
            return '';
        }
        const tmpDate = new Date(date.substring(0, 19));
        return new Date(tmpDate).pattern('yyyy-MM-dd HH:mm:ss');
    }
    /**
     * 判断是否有详情
     * 条件 （status==Asserted && eventsugg不是数字类型  && eventsugg不为null  && eventsugg不为空 && eventsugg ！= N/A.）
     * @param
     * @returns boolean 是/否
     */
    private _hasDetails(eventsugg: string, status?): boolean {
        let condition;
        if (status) {
            condition =
                eventsugg != null
                && typeof (eventsugg) !== 'number'
                && eventsugg !== ''
                && (eventsugg.indexOf('N/A') < 0)
                && status === 'Asserted';
        } else {
            condition =
                eventsugg != null
                && typeof (eventsugg) !== 'number'
                && eventsugg !== ''
                && (eventsugg.indexOf('N/A') < 0);
        }
        if (condition) {
            return true;
        } else {
            return false;
        }
    }
    /**
     * 将字符串按照一定的规则转换成字符串数组
     * @param str 原始字符串数据
     */
    private _dealDetails(str, status?): string[] {
        let hasDeatil = false;
        if (status) {
            hasDeatil = this._hasDetails(str, status);
        } else {
            hasDeatil = this._hasDetails(str, false);
        }
        if (!hasDeatil) { return; }
        const htmlValue: string[] = [];
        const list = str.split('@#AB;');
        list.forEach((strValue: string) => {
            htmlValue.push(strValue);
        });
        return htmlValue;
    }
    // 整合查询条件
    private _getParams(params) {
        if (params.Skip === 1) {
            this.queryParams.Skip = 0;
        } else {
            this.queryParams.Skip = (params.Skip - 1) * this.pageSize.size;
        }
        this.queryParams.Top = params.Top;
        this.queryParams.Level = params.Level;
        this.queryParams.Subject = params.Subject;
        // 产生时间
        const timeType = this.conditionsTitle[2].selected.id;
        if (timeType === 2) {
            this._caculateTimeGap(1);
        } else if (timeType === 3) {
            this._caculateTimeGap(7);
        } else if (timeType === 4) {
            this._caculateTimeGap(30);
        } else if (timeType === 5) {
            // 自定义查询日期
            this.value1.begin = params.BeginTime;
            this.value1.end = params.EndTime;
            this.queryParams.BeginTime = this._dateToStr(params.BeginTime, 1);
            this.queryParams.EndTime = this._dateToStr(params.EndTime, 0);
        } else {
            this.queryParams.BeginTime = '';
            this.queryParams.EndTime = '';
        }
        this.queryParams.SearchString = params.SearchString;
        this.searchValue = params.SearchString;
    }
    /**
     * 计算指定的日期前的时间
     * @param day 指定的几天前 如1，代表一天前 number
     */
    private _caculateTimeGap(day: number) {
        const currentAlarmEndTime = sessionStorage.getItem('DateTime').slice(0, 19);
        const sysDate = currentAlarmEndTime.replace('T', ' ');
        this.queryParams.EndTime = sysDate;
        const dateArray = sysDate.split(' ');
        if (day === 1) {
            this.queryParams.BeginTime = dateArray[0] + ' 00:00:00';
        } else {
            const splitDatePre = dateArray[0].split('-');
            const dateTime = new Date(Date.UTC(Number(splitDatePre[0]), Number(splitDatePre[1]) - 1, Number(splitDatePre[2])));
            let dateTimeStr = dateTime.setDate(dateTime.getUTCDate() - day);
            dateTimeStr = new Date(dateTimeStr).pattern('yyyy-MM-dd');
            this.queryParams.BeginTime = dateTimeStr + ' ' + dateArray[1];
        }
    }
    /**
     * 将时间转换成字符串
     * @param date 要转换的时间
     * @param isStart 开始还是结束标志 1 开始 0 结束
     * @returns 返回转换后的时间字符串
     */
    private _dateToStr(date, isStart: number) {
        const y = date.getFullYear();
        let m = (date.getMonth() + 1).toString();
        let d = date.getDate();
        if (m < 10) {
            m = `0${m}`;
        }
        if (d < 10) {
            d = `0${d}`;
        }
        if (isStart) {
            return `${y}-${m}-${d} 00:00:00`;
        } else {
            return `${y}-${m}-${d} 23:59:59`;
        }
    }
    /**
     * 过滤掉对象中的空值(告警级别、主题类型多选：数组清空null，日志范围单选：清空null与''，这种情况无需往后台传对应参数)
     * @param obj 要过滤的对象集合
     * @returns 过滤后的对象
     */
    private _deleteNULL(obj: object) {
        for (const key in obj) {
            if (
                Object.prototype.hasOwnProperty.call(obj, key)
                && (obj[key] === null || obj[key] === '' || obj[key].length === 0 || obj[key][0] === null)
            ) {
                delete obj[key];
            }
        }
        return obj;
    }
}
