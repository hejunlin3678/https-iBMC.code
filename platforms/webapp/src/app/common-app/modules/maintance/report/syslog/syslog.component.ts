import { Component, OnInit, TemplateRef } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormControl } from '@angular/forms';
import { TiValidators, TiFileItem, TiFileInfo, TiModalService, TiTableColumns, TiTableSrcData, TiMessageService } from '@cloud/tiny3';
import { SyslogService } from './services';
import { getMessageId } from 'src/app/common-app/utils';
import { SerAddrValidators } from 'src/app/common-app/utils/valid';
import {
    SystemLockService,
    ErrortipService,
    LoadingService,
    AlertMessageService,
    UserInfoService,
    TimeoutService
} from 'src/app/common-app/service';

@Component({
    selector: 'app-syslog',
    templateUrl: './syslog.component.html',
    styleUrls: ['./syslog.component.scss']
})
export class SyslogComponent implements OnInit {
    constructor(
        private translate: TranslateService,
        private service: SyslogService,
        private user: UserInfoService,
        private tiModal: TiModalService,
        private alert: AlertMessageService,
        private loading: LoadingService,
        private errorTip: ErrortipService,
        private tiMessage: TiMessageService,
        private lockService: SystemLockService,
        private timeoutService: TimeoutService,
    ) { }
    public hasPermission = this.user.hasPrivileges(['OemSecurityMgmt']);
    public isRFC3164: boolean = false;
    public syslogBtn: boolean = true;
    public showClient: boolean = false;
    public showRoot: boolean = false;
    public dialogInstance = null;
    public file: any;
    public isSystemLock: boolean = true;
    public syslogCancel: boolean = false;

    // 根证书
    public oneWayInfo = {
        issuer: '',
        subject: '',
        validNotBefore: '',
        validNotAfter: '',
        serialNumber: '',
        crlState: false,
        listTime: '',
        crlURL: '',
        deleteCrlUrl: '/UI/Rest/Maintenance/SyslogNotification/DeleteCrl'
    };
    // 客户端证书
    public twoWayInfo = {
        issuer: '',
        subject: '',
        validNotBefore: '',
        validNotAfter: '',
        serialNumber: ''
    };
    public defaultData;
    public syslog = {
        label: 'ALARM_REPORT_SYSLOG_ENABLE',
        state: true,
        change: () => {
            this.changeBtnStatus();
        }
    };
    public showAlert = {
        type: 'warn',
        openAlert: false,
        label: this.translate.instant('ALARM_REPORT_SYSLOG_TLS')
    };
    public protocol = {
        label: 'ALARM_REPORT_SYSLOG_PROTOCOL',
        list: [{
            id: 'TLS',
            key: 'TLS',
        }, {
            id: 'TCP',
            key: 'TCP',
        }, {
            id: 'UDP',
            key: 'UDP',
        }],
        value: null,
        change: () => {
            if (this.protocol.value === 'TLS') {
                this.showAlert['openAlert'] = false;
            } else {
                this.showAlert['openAlert'] = true;
            }
            this.changeBtnStatus();
        }
    };
    public authMode = {
        label: 'ALARM_REPORT_SYSLOG_AUTHOR',
        list: [{
            id: 'OneWay',
            key: 'ALARM_REPORT_SYSLOG_AUTHOR_SINGLE',
        }, {
            id: 'TwoWay',
            key: 'ALARM_REPORT_SYSLOG_AUTHOR_DOUBLE',
        }],
        value: '',
        change: () => {
            this.changeBtnStatus();
        }
    };
    public messageFormat = {
        list: [{
            id: 'Custom',
            key: this.translate.instant('SERVICE_WBS_CRE_CUSTOM'),
        }, {
            id: 'RFC3164',
            key: 'RFC3164',
        }],
        select: () => {
            if (this.messageFormatSelect['key'] === 'RFC3164') {
                this.isRFC3164 = true;
                this.syslogFlagSelected = this.syslogFlag.list[2];
            } else {
                this.isRFC3164 = false;
            }
            this.changeBtnStatus();
        }
    };
    public messageFormatSelect: object = this.messageFormat.list[1];
    public syslogFlag = {
        label: 'ALARM_REPORT_SYSLOG_FLAG',
        list: [{
            id: 'BoardSN',
            key: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_1'),
        }, {
            id: 'ProductAssetTag',
            key: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_2'),
        }, {
            id: 'HostName',
            key: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_3'),
        }],
        select: (option) => {
            this.changeBtnStatus();
        }
    };
    public syslogFlagSelected: object = this.syslogFlag.list[1];
    public alarmLevel = {
        label: 'ALARM_EVENT_ALARM_LEVEL',
        list: [{
            id: 'None',
            key: '[NULL]'
        }, {
            id: 'Critical',
            key: this.translate.instant('ALARM_EVENT_CRITICAL')
        }, {
            id: 'Major',
            key: this.translate.instant('ALARM_EVENT_MAJOR')
        }, {
            id: 'Minor',
            key: this.translate.instant('ALARM_EVENT_MINOR')
        }, {
            id: 'Normal',
            key: this.translate.instant('ALARM_EVENT_NORMAL')
        }],
        select: (option) => {
            this.changeBtnStatus();
        }
    };
    public alarmLevelSelected: object = this.alarmLevel.list[4];
    public upload = {
        label: 'ALARM_REPORT_SYSLOG_SERVICE_ROOT_AUTH'
    };
    // 表格
    public columns: TiTableColumns = [
        {
            title: 'EVENT_ORDER',
            width: '50px'
        },
        {
            title: 'ALARM_REPORT_SERVICE_ADDR',
            width: '225px'
        },
        {
            title: 'SERVICE_PORT',
            width: '158px'
        },
        {
            title: 'ALARM_REPORT_LOG_TYPE',
            width: '620px'
        },
        {
            title: 'ALARM_REPORT_CURRENT_STATUS',
            width: '105px'
        },
        {
            title: 'COMMON_OPERATE',
            width: '165px'
        }
    ];
    public srcData: TiTableSrcData = {
        data: [],
        state: {
            searched: false,
            sorted: false,
            paginated: false
        }
    };
    public uploadText: string = 'COMMON_UPLOAD';
    // 上传按钮状态
    public uploadBtnStatus: boolean = true;
    public fileName: any;
    public errorTips: string = '';
    public isAddFileError: boolean = false;
    // 本地证书上传
    public cer = {
        label1: 'ALARM_REPORT_SYSLOG_CER_FILE',
        label2: 'SERVICE_WBS_DLOG_CRE_PWD',
        tip: 'ALARM_REPORT_SYSLOG_CER_TIP',
        value: '',
        url: '',
        filters: [{
            name: 'maxCount',
            params: [1]
        }, {
            name: 'type',
            params: ['.pfx,.p12']
        }, {
            name: 'maxSize',
            params: [1024 * 1024]
        }],
        autoUpload: false,
        removeItems: () => {
            this.uploadBtnStatus = true;
            this.isAddFileError = false;
        },
        addItemSuccess: (data: TiFileItem) => {
            this.uploadBtnStatus = false;
            const fileItems = data['file'];
            this.file = fileItems._file;
            this.fileName = fileItems.name;
            this.isAddFileError = false;
        },
        addItemFailed: (data: TiFileInfo) => {
            const validResults = data['validResults'];
            const validResultValue = validResults[0];
            this.isAddFileError = true;
            this.uploadBtnStatus = true;
            if (validResultValue === 'maxSize') {
                this.errorTips = 'IBMC_UPLOAD_FILE_1M_ERROR3';
            } else if (validResultValue === 'type') {
                this.errorTips = 'IBMC_UPLOAD_FILE_TYPE';
            }
        },
        save: (context) => {
            if (this.file) {
                this.sysLogClientCertificate(this.file, context);
            }
        }
    };
    // 根证书上传
    public rootUpload = {
        url: '/UI/Rest/Maintenance/SyslogNotification/ImportRootCertificate',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.getToken()
        },
        type: 'POST',
        filters: [{
            name: 'type',
            params: ['.crt,.cer,.pem']
        }, {
            name: 'maxSize',
            params: [100 * 1024]
        }],
        autoUpload: true,
        alias: 'Content',
        successItems: (data: TiFileItem) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.saveLog();
        },
        addItemFailed: (data: TiFileInfo) => {
            const validResults = data['validResults'];
            const validResultValue = validResults[0];
            if (validResultValue === 'maxSize') {
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_UPLOAD_FILE_MORN_1M_ERROR4' });
            } else if (validResultValue === 'type') {
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FILE_SUFFIX_CER_PEM_CRT' });
            }
        },
        errorItems: ((fileItem) => {
            const res = fileItem.response;
            const errorId = getMessageId(res)[0].errorId;
            const errorMessage = this.errorTip.getErrorMessage(errorId) || 'COMMON_FAILED';
            if (errorId === 'NoValidSession') {
                this.timeoutService.error401.next(errorId);
            }
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
        }),
    };
    public tempTableData = [];
    // 表格操作
    public tableChange(event) {
        switch (event.key) {
            case 'save':
                const isValid =
                    (event.value.addrControl.errors
                        ? event.value.addrControl.errors.pattern.tiErrorMessage
                        : event.value.addrControl.errors
                    )
                    || event.value.portControl.errors;
                if (isValid) {
                    return;
                }
                event.value.isEdit = false;
                this.save(event.value);
                break;
            case 'cancel':
                this.cancel(event.value);
                event.value.isEdit = false;
                break;
            case 'edit':
                this.tempTableData[event.value.num - 1] = {
                    num: event.value.num,
                    statusValue: event.value.statusValue,
                    serviceAddr: event.value.serviceAddr,
                    port: event.value.port,
                    itemSelect: event.value.itemSelect,
                    LogType: [{
                        id: 'OperationLog',
                        text: 'OPERATE_LOG',
                        checked: false
                    }, {
                        id: 'SecurityLog',
                        text: 'SERCURITY_LOG',
                        checked: false
                    }, {
                        id: 'EventLog',
                        text: 'EVENT_LOG',
                        checked: false
                    }]
                };
                this.copy(this.tempTableData[event.value.num - 1].LogType, event.value.LogType);
                event.value.isEdit = true;
                break;
            case 'test':
                // 若参数未保存，弹框提示。
                if (!this.syslogBtn) {
                    const syslogTestModel = this.tiMessage.open({
                        id: 'syslogTestModel',
                        type: 'prompt',
                        okButton: {
                            show: true,
                            autofocus: false,
                            click() {
                                syslogTestModel.close();
                            }
                        },
                        cancelButton: {
                            show: false
                        },
                        title: this.translate.instant('COMMON_CONFIRM'),
                        content: this.translate.instant('ALARM_REPORT_SYSLOG_TEST')
                    });
                } else {
                    this.test(event.value);
                }
                break;
            default: return;
        }
    }
    public btnCancel(context) {
        context.close();
    }
    public copy(target, source) {
        for (const item of target) {
            for (const key of source) {
                if (item.id === key.id) {
                    item.checked = key.checked;
                    break;
                }
            }
        }
    }
    // 保存
    public save(data) {
        const params = {
            SyslogServerList: [{
                ID: data.num - 1,
                Enabled: data.statusValue,
                IP: data.serviceAddr,
                Port: parseInt(data.port, 10),
                LogType: this._resetLogType(data.LogType)
            }]
        };
        this.loading.state.next(true);
        this.service.sysLogSave(params).subscribe((res) => {
            const response = res['body'];
            let syslogServerList = response?.SyslogServerList;
            if (response.error) {
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                syslogServerList = response['data']?.SyslogServerList;
            } else {
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            }
            const memberId = data.num - 1;
            if (syslogServerList && syslogServerList.length > 0) {
                syslogServerList.forEach((item) => {
                    if (Number(item.ID) === Number(memberId)) {
                        data.statusValue = item.Enabled;
                        data.serviceAddr = item.IP;
                        data.port = item.Port === 0 ? null : item.Port;
                        this.copy(data.LogType, item.LogType);
                        data.logType = this.setLog();
                        this._setChecked(data.logType, item.LogType);
                        data.itemSelect = this._setLogTypeSelect(item.LogType);
                    }
                });
            }
            this.loading.state.next(false);
        }, (error) => {
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            this.loading.state.next(false);
        });
    }
    // 取消
    public cancel(data) {
        data.port = this.tempTableData[data.num - 1].port;
        data.statusValue = this.tempTableData[data.num - 1].statusValue;
        data.serviceAddr = this.tempTableData[data.num - 1].serviceAddr;
        data.port = this.tempTableData[data.num - 1].port;
        this.copy(data.LogType, this.tempTableData[data.num - 1].LogType);
        data.itemSelect = this.tempTableData[data.num - 1].itemSelect;
        this.tempTableData[data.num - 1] = null;
    }
    // 测试
    public test(data) {
        const params = {
            ID: Number(data.num - 1)
        };
        this.loading.state.next(true);
        this.service.sysLogSendTest(params).subscribe((res) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.loading.state.next(false);
        }, (error) => {
            const errorMessage = this.errorTip.getErrorMessage(getMessageId(error.error)[0].errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            this.loading.state.next(false);
        });
    }
    private _resetLogType(arr) {
        const result = [];
        arr.forEach((item) => {
            if (item.checked) {
                result.push(item.id);
            }
        });
        return result;
    }
    public sysLogClientCertificate(file: string, context) {
        const clientFormData = new FormData();
        clientFormData.append('Content', file, this.fileName);
        if (this.cer.value) {
            clientFormData.append('Password', this.cer.value);
        }
        this.service.imporCustomCertificate(clientFormData, {
            headers: {
                From: 'WebUI',
                Accept: '*/*',
                'X-CSRF-Token': this.user.getToken()
            }
        }).subscribe(() => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            context.close();
            this.saveLog();
        }, (err) => {
            const errorId = getMessageId(err.error)[0].errorId;
            if (errorId.indexOf('EncryptionAlgorithmIsWeak') > -1) {
                this.alert.eventEmit.emit({ type: 'error', label: 'ALARM_REPORT_SYSLOG_MD5_CARD_FAILED' });
            } else {
                const errorMessage = this.errorTip.getErrorMessage(errorId) || 'COMMON_FAILED';
                this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            }
            context.close();
        });
    }
    // 吊销证书上传结果
    public saveLog() {
        const saveParam = {};
        if (this.defaultData?.TransmissionProtocol !== this.protocol.value) {
            saveParam['TransmissionProtocol'] = this.protocol.value;
        }
        if (this.defaultData?.AuthenticateMode !== this.authMode.value) {
            saveParam['AuthenticateMode'] = this.authMode.value;
        }
        if (JSON.stringify(saveParam) !== '{}') {
            this.service.sysLogSave(saveParam).subscribe((res) => {
                this.getInfo();
            });
        } else {
            this.getInfo();
        }
    }
    ngOnInit(): void {
        this.getInfo();
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.isSystemLock !== res) {
                this.isSystemLock = res;
            }
        });
    }
    public getInfo() {
        this.loading.state.next(true);
        this.service.getSyslogInfo().subscribe((res) => {
            this.defaultData = res['body'];
            const response = res['body'];
            this.syslog.state = response.EnableState;
            this.protocol.value = response.TransmissionProtocol;
            if (this.protocol.value === 'TLS') {
                this.showAlert['openAlert'] = false;
            } else {
                this.showAlert['openAlert'] = true;
            }
            this.authMode.value = response.AuthenticateMode;
            this.alarmLevelSelected = this._getItem(response.AlarmSeverity, this.alarmLevel.list);
            this.syslogFlagSelected = this._getItem(response.ServerIdentity, this.syslogFlag.list);
            this.messageFormatSelect = this._getItem(response.MessageFormat, this.messageFormat.list);
            if (response.MessageFormat === 'RFC3164') {
                this.isRFC3164 = true;
            } else {
                this.isRFC3164 = false;
            }
            if (response.RootCertificate) {
                this.oneWayInfo.issuer = response.RootCertificate.Issuer;
                this.oneWayInfo.subject = response.RootCertificate.Subject;
                this.oneWayInfo.validNotBefore = response.RootCertificate.ValidNotBefore;
                this.oneWayInfo.validNotAfter = response.RootCertificate.ValidNotAfter;
                this.oneWayInfo.serialNumber = response.RootCertificate.SerialNumber;
                this.oneWayInfo.crlState = response.RootCertificate.CrlVerificationEnabled;
                if (response.RootCertificate.CrlValidFrom && response.RootCertificate.CrlValidTo) {
                    this.oneWayInfo.listTime = `${response.RootCertificate.CrlValidFrom} - ${response.RootCertificate.CrlValidTo}`;
                } else {
                    this.oneWayInfo.listTime = '--';
                }
                this.showRoot = true;
            } else {
                this.showRoot = false;
            }
            if (response.ClientCertificate) {
                this.twoWayInfo.issuer = response.ClientCertificate.Issuer;
                this.twoWayInfo.subject = response.ClientCertificate.Subject;
                this.twoWayInfo.validNotBefore = response.ClientCertificate.ValidNotBefore;
                this.twoWayInfo.validNotAfter = response.ClientCertificate.ValidNotAfter;
                this.twoWayInfo.serialNumber = response.ClientCertificate.SerialNumber;
                this.showClient = true;
            } else {
                this.showClient = false;
            }

            // 有表格数据
            if (response.SyslogServerList && response.SyslogServerList.length > 0 && !this.syslogCancel) {
                this.srcData.data = [];
                response.SyslogServerList.forEach((item, idx) => {
                    const obj = {};
                    obj['num'] = Number(item.ID) + 1;
                    obj['serviceAddr'] = item.IP;
                    obj['statusValue'] = item.Enabled;
                    obj['port'] = item.Port === 0 ? null : item.Port;
                    obj['LogType'] = [{
                        id: 'OperationLog',
                        text: 'OPERATE_LOG',
                        checked: false
                    }, {
                        id: 'SecurityLog',
                        text: 'SERCURITY_LOG',
                        checked: false
                    }, {
                        id: 'EventLog',
                        text: 'EVENT_LOG',
                        checked: false
                    }];
                    obj['isEdit'] = false;
                    obj['itemSelect'] = this._setLogTypeSelect(item.LogType);
                    obj['validationAddr'] = {
                        tip: this.translate.instant('COMMON_IPV4_IPV6_TLS_DOMIN'),
                        tipPosition: 'right',
                        errorMessage: {
                            pattern: this.translate.instant('COMMON_FORMAT_ERROR')
                        }
                    };
                    obj['addrControl'] = new UntypedFormControl(obj['serviceAddr'], [SerAddrValidators.validateIp46AndDomin()]);
                    obj['validationPort'] = {
                        tipPosition: 'right',
                        errorMessage: {
                            number: this.translate.instant('ALARM_REPORT_RANGESIZE'),
                            digits: this.translate.instant('ALARM_REPORT_RANGESIZE'),
                            rangeValue: this.translate.instant('ALARM_REPORT_RANGESIZE')
                        }
                    };
                    obj['portControl'] = new UntypedFormControl(obj['description'],
                        [TiValidators.required, TiValidators.number, TiValidators.digits, TiValidators.rangeValue(1, 65535)]);
                    this._setChecked(obj['LogType'], item.LogType);

                    this.srcData.data.push(obj);
                });
            }
            this.syslogCancel = false;
            this.syslogBtn = true;
            this.loading.state.next(false);
        }, (error) => {
            this.loading.state.next(false);
        });
    }

    public setLog() {
        return [{
            id: 'OperationLog',
            text: 'OPERATE_LOG',
            checked: false
        }, {
            id: 'SecurityLog',
            text: 'SERCURITY_LOG',
            checked: false
        }, {
            id: 'EventLog',
            text: 'EVENT_LOG',
            checked: false
        }];
    }
    public changeBtnStatus() {
        const isEqual = this.syslog.state === this.defaultData?.EnableState &&
            this.protocol.value === this.defaultData?.TransmissionProtocol &&
            this.authMode.value === this.defaultData?.AuthenticateMode &&
            this.alarmLevelSelected['id'] === this.defaultData?.AlarmSeverity &&
            this.syslogFlagSelected['id'] === this.defaultData?.ServerIdentity &&
            this.messageFormatSelect['id'] === this.defaultData?.MessageFormat;
        if (isEqual) {
            this.syslogBtn = true;
        } else {
            this.syslogBtn = false;
        }
    }
    // 保存上面的数据
    public saveBtn() {
        const saveParam = {};
        if (this.syslog.state !== this.defaultData?.EnableState) {
            saveParam['EnableState'] = this.syslog.state;
        }
        if (this.protocol.value !== this.defaultData?.TransmissionProtocol) {
            saveParam['TransmissionProtocol'] = this.protocol.value;
        }
        if (this.authMode.value !== this.defaultData?.AuthenticateMode) {
            saveParam['AuthenticateMode'] = this.authMode.value;
        }
        if (this.alarmLevelSelected['id'] !== this.defaultData?.AlarmSeverity) {
            saveParam['AlarmSeverity'] = this.alarmLevelSelected['id'];
        }
        if ((this.syslogFlagSelected['id'] !== this.defaultData?.ServerIdentity)
            && this.messageFormatSelect['id'] !== 'RFC3164') {
            saveParam['ServerIdentity'] = this.syslogFlagSelected['id'];
        }
        if (this.messageFormatSelect['id'] !== this.defaultData?.MessageFormat) {
            saveParam['MessageFormat'] = this.messageFormatSelect['id'];
        }
        // 不为空
        if (JSON.stringify(saveParam) === '{}') {
            this.getInfo();
            return;
        }
        this.loading.state.next(true);
        this.service.sysLogSave(saveParam).subscribe((res) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.getInfo();
            this.syslogBtn = true;
            this.loading.state.next(false);
        }, (error) => {
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            this.syslogBtn = false;
            this.loading.state.next(false);
        });
    }
    public syslogCancelBtn() {
        this.syslogCancel = true;
        this.getInfo();
    }
    public uploadDialogFun(context: TemplateRef<any>) {
        this.isAddFileError = false;
        this.uploadBtnStatus = true;
        this.cer.value = '';
        this.dialogInstance = this.tiModal.open(context, {
            id: 'uploadDialog'
        });
    }

    private _getItem(id: string, arr: object[]) {
        for (const key of arr) {
            if (key['id'] === id) {
                return key;
            }
        }
    }
    private _setChecked(arr, selected: [] = []) {
        for (const key of arr) {
            for (const item of selected) {
                if (key['id'] === item) {
                    key.checked = true;
                    break;
                }
            }
        }
    }
    private _setLogTypeSelect(arr: []) {
        if (arr.length === 0) {
            return;
        }
        let result = '';
        arr.forEach((item) => {
            result += this._filterLogType(item) + ' + ';
        });
        return result.slice(0, -2);
    }
    private _filterLogType(type: string) {
        let zh = '';
        switch (type) {
            case 'OperationLog':
                zh = this.translate.instant('OPERATE_LOG');
                break;
            case 'SecurityLog':
                zh = this.translate.instant('SERCURITY_LOG');
                break;
            case 'EventLog':
                zh = this.translate.instant('EVENT_LOG');
                break;
            default:
        }
        return zh;
    }
}
