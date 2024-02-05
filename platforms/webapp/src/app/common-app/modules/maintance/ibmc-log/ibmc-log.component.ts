import { Component, OnInit } from '@angular/core';
import { ITabs } from '../alarm-event/domain';
import { TiTableColumns, TiTableSrcData, TiTableRowData } from '@cloud/tiny3';
import { IbmcLogService } from './services';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService, CommonService, UserInfoService } from 'src/app/common-app/service';

@Component({
    selector: 'app-ibmclog',
    templateUrl: './ibmc-log.component.html',
    styleUrls: ['./ibmc-log.component.scss']
})
export class IbmclogComponent implements OnInit {

    constructor(
        private service: IbmcLogService,
        private loading: LoadingService,
        private commonService: CommonService,
        private user: UserInfoService,
        private translate: TranslateService
    ) {
    }
    public type = 'operate';
    public isPrivileges = this.user.hasPrivileges(['OemSecurityMgmt']);
    public isSystemLock: boolean = this.user.systemLockDownEnabled;
    public tabs: ITabs[] = [{
        id: 'operate',
        title: 'OPERATE_LOG',
        active: true
    }, {
        id: 'running',
        title: 'RUNNING_LOG',
        active: false
    }, {
        id: 'security',
        title: 'SERCURITY_LOG',
        active: false
    }];
    public operateCol = [{
        title: 'EVENT_ORDER',
        width: '5%'
    },
    {
        title: 'BMC_LOG_INTERFACE',
        width: '5%'
    },
    {
        title: 'BMC_LOG_USER',
        width: '8%'
    },
    {
        title: 'BMC_LOG_IP',
        width: '8%'
    },
    {
        title: 'EVENT_ENGENDER_TIME',
        width: '10%'
    },
    {
        title: 'FDM_INFO',
        width: '20%'
    }];
    public runningCol = [{
        title: 'EVENT_ORDER',
        width: '5%'
    },
    {
        title: 'EVENT_LEVEL',
        width: '5%'
    },
    {
        title: 'EVENT_ENGENDER_TIME',
        width: '10%'
    },
    {
        title: 'FDM_INFO',
        width: '20%'
    }];
    public securityCol = [{
        title: 'EVENT_ORDER',
        width: '5%'
    },
    {
        title: 'BMC_LOG_INTERFACE',
        width: '10%'
    },
    {
        title: 'BMC_LOG_HOST',
        width: '10%'
    },
    {
        title: 'EVENT_ENGENDER_TIME',
        width: '10%'
    },
    {
        title: 'FDM_INFO',
        width: '20%'
    }];
    public downloadTip = this.translate.instant('ALARM_EVENT_DOWNLOAD');
    public runningLog = this.translate.instant('RUNNING_LOG');
    public columns: TiTableColumns[] = this.operateCol;
    // 分页
    public currentPage: number = 1;
    public totalNumber: number = 0;
    public operateSize: number = 15;
    public runningSize: number = 15;
    public securitySize: number = 15;
    public pageSize: { options: number[], size: number; } = {
        options: [15, 30, 50],
        size: 15
    };
    public srcData: TiTableSrcData;
    public load: boolean = true;
    public downloadState: boolean = false;
    private data: TiTableRowData[] = [];
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    ngOnInit(): void {
        this.srcData = {
            data: this.data,
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
        // 判断操作日志权限
        if (!this.isPrivileges) {
            this.tabs = [{
                id: 'running',
                title: this.runningLog,
                active: true
            }];
            this.type = 'running';
            this.columns = this.runningCol;
            this.getInitData('running', {
                Skip: 0,
                Top: this.pageSize.size
            });
        } else {
            this.getInitData('operate', {
                Skip: 0,
                Top: this.pageSize.size
            });
        }
        this.loading.state.next(true);
    }
    // tab页切换触发
    public tabChange(isActive: boolean, type: string): void {
        const params = {
            Skip: 0,
            Top: this.pageSize.size
        };
        if (isActive && type === 'operate') {
            this.type = 'operate';
            this.columns = this.operateCol;
            params.Top = this.operateSize;
            this.pageSize.size = this.operateSize;
            this.getInitData('operate', params);
        } else if (isActive && type === 'running') {
            this.type = 'running';
            this.columns = this.runningCol;
            params.Top = this.runningSize;
            this.pageSize.size = this.runningSize;
            this.getInitData('running', params);
        } else if (isActive && type === 'security') {
            this.type = 'security';
            this.columns = this.securityCol;
            params.Top = this.securitySize;
            this.pageSize.size = this.securitySize;
            this.getInitData('security', params);
        }
        this.currentPage = 1;
        this.srcData.data = [];
        this.loading.state.next(true);
        this.load = true;
    }
    public getInitData(type, params) {
        this.loading.state.next(true);
        this.service.getLog(type, params).subscribe((res) => {
            const response = res['body'];
            this.totalNumber = response.TotalCount || 0;
            this.initData(1);
        }, () => {
            this.loading.state.next(false);
            this.load = false;
        });
    }
    // 分页改变
    public pageChange(pageParams) {
        if (this.type === 'operate') {
            this.operateSize = pageParams.size;
        } else if (this.type === 'running') {
            this.runningSize = pageParams.size;
        } else if (this.type === 'security') {
            this.securitySize = pageParams.size;
        }
        this.currentPage = pageParams.currentPage;
        this.totalNumber = pageParams.totalNumber;
        this.initData(this.currentPage);
        this.loading.state.next(true);
        this.load = true;
    }
    // 初始化数据
    initData(page) {
        this.currentPage = page;
        let size = this.pageSize.size;
        if (this.type === 'operate') {
            size = this.operateSize;
        } else if (this.type === 'running') {
            size = this.runningSize;
        } else if (this.type === 'security') {
            size = this.securitySize;
        }
        const maxCount = this.totalNumber;
        if (maxCount === 0) {
            this.loading.state.next(false);
            this.load = false;
            return;
        }
        const remainder = maxCount % size;
        let start = ((maxCount - size * page) > 0) ? (maxCount - size * page) : 0;
        if (start + size > maxCount) {
            if (maxCount <= size) {
                start = 0;
                size = maxCount;
            } else {
                start = maxCount - size;
            }
        } else if (start === 0 && remainder > 0) {
            // 查询最后一页
            size = remainder;
        }
        // redfish接口最大支持32条数据，大于32条需查询两次
        if (size > 32) {
            this.service.getLog(this.type, {
                Skip: start,
                Top: 32
            }).subscribe((data) => {
                const response = data['body'];
                const members = response.Logs;
                this.service.getLog(this.type, {
                    Skip: start + 32,
                    Top: size - 32
                }).subscribe((data2) => {
                    const res = data2['body'];
                    const items = this._sort(members.concat(res.Logs));
                    this.dataAssembly(items);
                });
            }, () => {
                this.loading.state.next(false);
                this.load = false;
            });
        } else {
            this.service.getLog(this.type, {
                Skip: start,
                Top: size
            }).subscribe((data) => {
                const items = this._sort(data['body'].Logs);
                this.dataAssembly(items);
            }, () => {
                this.loading.state.next(false);
                this.load = false;
            });
        }
    }
    // 渲染表格数据
    public dataAssembly(data) {
        const temp = [];
        if (this.type === 'operate') {
            data.forEach((item) => {
                if (!this._isNullOrUndfined(item.ID)) {
                    return;
                }
                temp.push({
                    Id: item.ID,
                    Interface: this._isNullOrUndfined(item.Interface) ? item.Interface : 'N/A',
                    User: this._isNullOrUndfined(item.UserName) ? item.UserName : 'N/A',
                    Address: this._isNullOrUndfined(item.IPAdress) ? item.IPAdress : 'N/A',
                    Created: this._isNullOrUndfined(item.Time) ? item.Time : 'N/A',
                    Message: this._isNullOrUndfined(item.Content) ? item.Content : 'N/A'
                });
            });
        } else if (this.type === 'running') {
            data.forEach((item) => {
                if (!this._isNullOrUndfined(item.ID)) {
                    return;
                }
                temp.push({
                    Id: item.ID,
                    Level: this._isNullOrUndfined(item.Level) ? item.Level : 'N/A',
                    Created: this._isNullOrUndfined(item.Time) ? item.Time : 'N/A',
                    Message: this._isNullOrUndfined(item.Content) ? item.Content : 'N/A'
                });
            });
        } else if (this.type === 'security') {
            data.forEach((item) => {
                if (!this._isNullOrUndfined(item.ID)) {
                    return;
                }
                temp.push({
                    Id: item.ID,
                    Interface: this._isNullOrUndfined(item.Interface) ? item.Interface : 'N/A',
                    Host: this._isNullOrUndfined(item.Host) ? item.Host : 'N/A',
                    Created: this._isNullOrUndfined(item.Time) ? item.Time : 'N/A',
                    Message: this._isNullOrUndfined(item.Content) ? item.Content : 'N/A'
                });
            });
        }
        this.load = false;
        this.srcData.data = temp;
        setTimeout(() => {
            this.loading.state.next(false);
        }, 1000);
    }
    // 下载日志
    public downloadLog(type) {
        let url = '';
        if (type === 'operate') {
            url = `/UI/Rest/Maintenance/DownloadOperationLog`;
        }
        if (type === 'running') {
            url = `/UI/Rest/Maintenance/DownloadRunLog`;
        }
        if (type === 'security') {
            url = `/UI/Rest/Maintenance/DownloadSecurityLog`;
        }
        this.downloadState = true;
        this.service.downloadLog(url).subscribe((data) => {
            const response = data['body'];
            this.getTaskState(response.url);
        }, () => {
            this.downloadState = false;
        });
    }
    // 获取下载进度
    public getTaskState(url) {
        this.service.getTaskStatus(url).subscribe((res) => {
            const taskData = res['body'];
            if (taskData.prepare_progress === 100 && taskData.downloadurl) {
                this.commonService.restDownloadFile(taskData.downloadurl).then(() => {
                    this.downloadState = false;
                }, () => {
                    this.downloadState = false;
                });
            } else {
                this.getTaskState(url);
            }
        }, () => {
            this.downloadState = false;
        });
    }
    // 排序
    private _sort(arr) {
        arr.sort((a, b) => {
            return b.ID - a.ID;
        });
        return arr;
    }
    // not Null&undefined
    private _isNullOrUndfined(str) {
        if (str !== undefined || str !== null) {
            return true;
        } else {
            return false;
        }
    }
}
