import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormBuilder, UntypedFormGroup, UntypedFormControl } from '@angular/forms';
import { TiValidationConfig, TiValidators, TiTableColumns, TiTableSrcData } from '@cloud/tiny3';
import { MultVaild } from '../../../../utils/multValid';
import { TrapService } from './service';
import { TrapTableInfo } from './model/trapTable';
import { AlertMessageService, LoadingService, UserInfoService, ErrortipService, SystemLockService } from 'src/app/common-app/service';
import { SerAddrValidators } from 'src/app/common-app/utils/valid';
import { getMessageId } from 'src/app/common-app/utils';

@Component({
    selector: 'app-trap',
    templateUrl: './trap.component.html',
    styleUrls: ['./trap.component.scss']
})
export class TrapComponent implements OnInit {
    constructor(
        private translate: TranslateService,
        private fb: UntypedFormBuilder,
        private service: TrapService,
        private alert: AlertMessageService,
        private loading: LoadingService,
        private errorTipService: ErrortipService,
        private user: UserInfoService,
        private lockService: SystemLockService
    ) {
        // 获取是否开启密码校验
        this.getPwdCode();
        this.trapForm = this.fb.group({
            trapEnable: [null, []],
            v3UserSel: [null, []],
            trapVersionSel: [null, []],
            trapModeSel: [null, []],
            trapDomainSel: [null, []],
            group: [null, []],
            confirmGroup: [null, [MultVaild.equalTo('group')]],
            emailLevel: [null, []]
        });
    }
    public isSystemLock: boolean = this.user.systemLockDownEnabled;
    public hasPermission = this.user.hasPrivileges(['OemSecurityMgmt']);
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public trapCancel: boolean = false;
    public isIRM210: boolean = false;

    // 表格的表头
    public columns: TiTableColumns = [
        {
            title: ''
        },
        {
            title: 'EVENT_ORDER',
            width: '5%'
        },
        {
            title: 'TRAP_SERVER_ADDR',
            width: '20%'
        },
        {
            title: 'TRAP_PORT',
            width: '15%'
        },
        {
            title: 'TRAP_FORWARD',
            width: '20%',
            show: false
        },
        {
            title: 'TRAP_CURRENT_STATUS',
            width: '15%'
        },
        {
            title: 'COMMON_OPERATE',
            width: '20%'
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
    // 初始化属性
    public showAlert = {
        type: 'warn',
        openAlert: false,
        label: this.translate.instant('SERVICE_REDUCE_SECURITY')
    };
    public trap = {
        state: false,
        change: () => {
            this.changeBtnState();
        }
    };
    public trapVersion = {
        list: [{
            id: 'SNMPv1',
            key: this.translate.instant('TRAP_SNMP_V1'),
        }, {
            id: 'SNMPv2c',
            key: this.translate.instant('TRAP_SNMP_V2'),
        }, {
            id: 'SNMPv3',
            key: this.translate.instant('TRAP_SNMP_V3'),
        }],
        change: (sel) => {
            if (sel.id === 'SNMPv3') {
                this.groupShow = false;
                this.v3Show = true;
                this.showAlert['openAlert'] = false;
            } else {
                this.groupShow = true;
                this.v3Show = false;
                this.showAlert['openAlert'] = true;
            }
            this.changeBtnState();
        }
    };
    public v3User = {
        list: [],
        change: () => {
            this.changeBtnState();
        }
    };
    public trapMode = {
        label: 'ALARM_REPORT_TRAP_MODE',
        list: [{
            id: 'OID',
            key: this.translate.instant('TRAP_OID_MODE'),
        }, {
            id: 'EventCode',
            key: this.translate.instant('TRAP_EVENT_CODE'),
        }, {
            id: 'PreciseAlarm',
            key: this.translate.instant('TRAP_PRECISE_ALARM')
        }],
        change: (sel) => {
            if (sel.id === 'EventCode') {
                this.dominShow = false;
            } else {
                this.dominShow = true;
            }
            this.changeBtnState();
        }
    };
    public trapDomain = {
        list: [{
            id: 'BoardSN',
            key: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_1')
        }, {
            id: 'ProductAssetTag',
            key: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_2')
        }, {
            id: 'HostName',
            key: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_3')
        }],
        change: () => {
            this.changeBtnState();
        }
    };
    public sendEmailLevel = {
        list: [{
            id: 'None',
            label: '[NULL]'
        }, {
            id: 'Critical',
            label: this.translate.instant('ALARM_EVENT_CRITICAL')
        }, {
            id: 'Major',
            label: this.translate.instant('ALARM_EVENT_MAJOR')
        }, {
            id: 'Minor',
            label: this.translate.instant('ALARM_EVENT_MINOR')
        }, {
            id: 'Normal',
            label: this.translate.instant('ALARM_EVENT_NORMAL')
        }],
        change: () => {
            this.changeBtnState();
        }
    };
    public group = {
        change: () => {
            this.trapForm.get('confirmGroup').updateValueAndValidity();
        }
    };

    public confirmGroupName = {
        change: () => {
            this.trapForm.get('confirmGroup').updateValueAndValidity();
            this.changeBtnState();
        }
    };
    public v3Info = null;
    public btnSaveState: boolean = true;
    public v3Show: boolean = false;
    public groupShow: boolean = false;
    public dominShow: boolean = false;
    public validGroup: TiValidationConfig;
    // 确认团体名错误提示设置
    public validconfirmGroup: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            equalTo: this.translate.instant('TRAP_GROUP_NOEQUAL')
        }
    };
    public trapForm: UntypedFormGroup;
    // 用来对比的表单数据
    public renderData;
    public defaultTable: TrapTableInfo;
    // 服务器地址校验
    public validationAddr: TiValidationConfig = {
        tip: this.translate.instant('COMMON_IPV4_IPV6_DOMIN'),
        tipPosition: 'right',
        errorMessage: {
            pattern: this.translate.instant('COMMON_FORMAT_ERROR')
        }
    };
    // 端口校验
    public portValid = {
        tipPosition: 'right',
        errorMessage: {
            number: this.translate.instant('ALARM_REPORT_RANGESIZE'),
            digits: this.translate.instant('ALARM_REPORT_RANGESIZE'),
            rangeValue: this.translate.instant('ALARM_REPORT_RANGESIZE')
        }
    };
    ngOnInit(): void {
        // 获取trap信息
        if (this.isSystemLock || !this.isConfigureComponentsUser) {
            this.trapForm.root.get('trapEnable').disable();
            this.trapForm.root.get('group').disable();
            this.trapForm.root.get('confirmGroup').disable();
        }
        // 防止切换页签时禁用会慢几秒
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.isSystemLock !== res) {
                this.isSystemLock = res;
            }
            if (this.isSystemLock || !this.isConfigureComponentsUser) {
                this.trapForm.root.get('trapEnable').disable();
                this.trapForm.root.get('group').disable();
                this.trapForm.root.get('confirmGroup').disable();
            } else {
                this.trapForm.root.get('trapEnable').enable();
                this.trapForm.root.get('group').enable();
                this.trapForm.root.get('confirmGroup').enable();
            }
        });
        this.getTrapInfo();
        if (this.trapForm.root.get('trapVersionSel').value && this.trapForm.root.get('trapVersionSel').value.id !== 'SNMPv3') {
            this.showAlert['openAlert'] = true;
        } else {
            this.showAlert['openAlert'] = false;
        }
    }

    // 获取trap初始信息
    public getTrapInfo() {
        this.loading.state.next(true);
        this.service.getTrapInfo().subscribe((res) => {
            const response = res['body'];
            if (response.Version === 'SNMPv3') {
                this.v3Show = true;
                this.groupShow = false;
            } else {
                this.v3Show = false;
                this.groupShow = true;
            }
            if (response.Mode === 'EventCode') {
                this.dominShow = false;
            } else {
                this.dominShow = true;
            }
            this.v3Info = response.SNMPv3User;
            this.v3User.list = this._extendV3(response.AvailableUsers);
            for (const item of this.v3User.list) {
                if (item.id === this.v3Info) {
                    this.trapForm.patchValue({
                        v3UserSel: item
                    });
                }
            }
            this.renderData = {
                enable: response.Enabled,
                version: this._filter(response.Version, this.trapVersion.list),
                mode: this._filter(response.Mode, this.trapMode.list),
                domin: this._filter(response.ServerIdentity, this.trapDomain.list),
                groupName: '',
                groupReName: '',
                level: this._filter(response.AlarmSeverity, this.sendEmailLevel.list),
            };
            const trapData = response.TrapServerList;
            if (!this.trapCancel) {
                this.setTrapTable(response.Mode, trapData);
            }
            this.assign();
            this.trapCancel = false;
            this.loading.state.next(false);
        }, () => {
            this.loading.state.next(false);
        });
    }
    // 设置表格数据
    public setTrapTable(mode, trapData) {
        const arr = [];
        for (const item of trapData) {
            const isShow = item.DisplayKeywords;
            const separator = item.MessageDelimiter;
            const tempArr = [];
            const timeSel = item.MessageContent.indexOf('Time') > -1;
            const sensorSel = item.MessageContent.indexOf('SensorName') > -1;
            const serverSel = item.MessageContent.indexOf('Severity') > -1;
            const eventSel = item.MessageContent.indexOf('EventCode') > -1;
            const descSel = item.MessageContent.indexOf('EventDesc') > -1;
            const reportContent = [{
                label: 'TRAP_SYS_DATE',
                id: 'Time',
                checked: false
            }, {
                label: 'TRAP_SENSOR_NAME',
                id: 'SensorName',
                checked: false
            }, {
                label: 'TRAP_SYS_SEVERITY',
                id: 'Severity',
                checked: false
            }, {
                label: 'TRAP_SYS_EVENT_CODE',
                id: 'EventCode',
                checked: false
            }, {
                label: 'TRAP_SYS_EVENT_REMARK',
                id: 'EventDescription',
                checked: false
            }];
            reportContent[0].checked = timeSel;
            reportContent[1].checked = sensorSel;
            reportContent[2].checked = serverSel;
            reportContent[3].checked = eventSel;
            reportContent[4].checked = descSel;
            const repalceList = [{
                id: ';',
                key: ';',
                repalceListId: 'repalceListId0',
            }, {
                id: '/',
                key: '/',
                repalceListId: 'repalceListId1',
            }, {
                id: '-',
                key: '-',
                repalceListId: 'repalceListId2',
            }, {
                id: ',',
                key: ',',
                repalceListId: 'repalceListId3',
            }];
            const showList = [{
                id: true,
                key: this.translate.instant('COMMON_YES'),
                showListId: 'showListId0',
            }, {
                id: false,
                key: this.translate.instant('COMMON_NO'),
                showListId: 'showListId1',
            }];
            if (isShow) {
                if (timeSel) {
                    tempArr.push('Time:1970-01-01 01:52:00');
                }
                if (sensorSel) {
                    tempArr.push('Sensor:test sensor');
                }
                if (serverSel) {
                    tempArr.push('Severity:Assertion Normal');
                }
                if (eventSel) {
                    tempArr.push('Code:0x00000001');
                }
                if (descSel) {
                    tempArr.push('Description:sensor test event');
                }
            } else {
                if (timeSel) {
                    tempArr.push('1970-01-01 01:52:00');
                }
                if (sensorSel) {
                    tempArr.push('test sensor');
                }
                if (serverSel) {
                    tempArr.push('Assertion Normal');
                }
                if (eventSel) {
                    tempArr.push('0x00000001');
                }
                if (descSel) {
                    tempArr.push('sensor test event');
                }
            }
            const obj = {};
            obj['isEdit'] = false;
            obj['isShowDetails'] = (mode === 'EventCode') ? true : false;
            obj['disable'] = false;
            obj['num'] = Number(item.ID) + 1;
            obj['serverAddr'] = item.IPAddress;
            obj['validationAddr'] = this.validationAddr;
            obj['addrControl'] = new UntypedFormControl(item.IPAddress, [SerAddrValidators.validateIp46AndDomin()]);
            obj['validationPort'] = this.portValid;
            obj['portControl'] = new UntypedFormControl(obj['port'],
                [TiValidators.required, TiValidators.number, TiValidators.digits, TiValidators.rangeValue(1, 65535)]);
            obj['serverAddrBak'] = item.IPAddress;
            obj['port'] = item.Port;
            obj['portBak'] = item.Port;
            obj['trans'] = item.BobEnabled;
            obj['status'] = item.Enabled;
            obj['transBak'] = item.BobEnabled;
            obj['statusBak'] = item.Enabled;
            obj['example'] = tempArr;
            obj['separator'] = separator;
            obj['keyWord'] = item.DisplayKeywords;
            obj['result'] = item.DisplayKeywords ? 'COMMON_YES' : 'COMMON_NO';
            obj['reportContent'] = reportContent;
            obj['repalceList'] = repalceList;
            obj['showList'] = showList;
            if (item.BobEnabled === undefined) {
                this.isIRM210 = true;
                this.columns[4].show = true;
                this.columns[2].width = '40%';
            } else {
                this.isIRM210 = false;
            }
            arr.push(obj);
        }
        this.loading.state.next(false);
        this.srcData.data = arr;
    }
    // 获取是否开启密码校验
    public getPwdCode() {
        this.service.getPwdCode().subscribe((res) => {
            const response = res['body'];
            if (response.PasswordComplexityCheckEnabled) {
                this.trapForm.get('group')
                    .setValidators([TiValidators.rangeSize(8, 32), MultVaild.pattern(), MultVaild.special(), MultVaild.noEmpty()]);
                this.validGroup = {
                    passwordConfig: {
                        validator: {
                            rule: 'new',
                            params: {},
                            message: {
                                rangeSize: this.translate.instant('TRAP_GROUP_RANGESIZE'),
                                pattern: this.translate.instant('COMMON_SPECIAL_CHARACTERS'),
                                special: this.translate.instant('COMMON_ALLOWING_CHARACTERS'),
                                noEmpty: this.translate.instant('TRAP_GROUP_NOSPACE')
                            }
                        }
                    },
                    errorMessage: {
                        rangeSize: this.translate.instant('COMMON_FORMAT_ERROR'),
                        pattern: this.translate.instant('COMMON_FORMAT_ERROR'),
                        special: this.translate.instant('COMMON_FORMAT_ERROR'),
                        noEmpty: this.translate.instant('COMMON_FORMAT_ERROR')
                    }
                };
            } else {
                this.trapForm.get('group')
                    .setValidators([TiValidators.rangeSize(1, 32), MultVaild.noEmpty()]);
                this.validGroup = {
                    tip: this.translate.instant('TRAP_COMMUNITY_NAME_TIPS'),
                    errorMessage: {
                        rangeSize: this.translate.instant('COMMON_FORMAT_ERROR'),
                        noEmpty: this.translate.instant('COMMON_FORMAT_ERROR')
                    },
                };
            }
            this.trapForm.get('group').updateValueAndValidity();
        });
    }
    // 判断按钮变化状态
    changeBtnState() {
        if (!this.trapForm.valid) {
            this.btnSaveState = true;
            return;
        }
        if (!this.renderData) {
            return;
        }
        const isCommonChange =
            this.trapForm.root.get('trapEnable').value === this.renderData['enable']
            && this.trapForm.root.get('trapVersionSel').value
            && (this.trapForm.root.get('trapVersionSel').value.id === this.renderData['version'].id)
            && this.trapForm.root.get('trapModeSel').value
            && (this.trapForm.root.get('trapModeSel').value.id === this.renderData['mode'].id)
            && this.trapForm.root.get('emailLevel').value
            && (this.trapForm.root.get('emailLevel').value.id === this.renderData['level'].id);

        const hasV3 = this.trapForm.root.get('v3UserSel').value
            && (this.trapForm.root.get('v3UserSel').value.id === this.v3Info);

        let hasGroup = this.trapForm.root.get('group').value === this.renderData['groupName'];
        let trapFlag = this.trapForm.root.get('trapDomainSel').value
            && (this.trapForm.root.get('trapDomainSel').value.id === this.renderData['domin'].id)
            && this.trapForm.root.get('confirmGroup').value === this.renderData['groupReName'];
        // 事件码模式不用校验主机标识
        if (this.trapForm.root.get('trapModeSel').value && this.trapForm.root.get('trapModeSel').value.id === 'EventCode') {
            trapFlag = true;
        }
        // v3用户不用校验团体名
        if (this.trapForm.root.get('trapVersionSel').value && this.trapForm.root.get('trapVersionSel').value.id === 'SNMPv3') {
            hasGroup = true;
        }
        const condation = isCommonChange && trapFlag && hasGroup;
        if (this.v3Show) {
            if (condation && hasV3) {
                this.btnSaveState = true;
            } else {
                this.btnSaveState = false;
            }
        } else {
            if (condation) {
                this.btnSaveState = true;
            } else {
                this.btnSaveState = false;
            }
        }
    }
    // 赋初始值
    public assign() {
        this.trapForm.patchValue({
            trapEnable: this.renderData['enable'],
            trapVersionSel: this.renderData['version'],
            trapModeSel: this.renderData['mode'],
            trapDomainSel: this.renderData['domin'],
            group: this.renderData['groupName'],
            confirmGroup: this.renderData['groupReName'],
            emailLevel: this.renderData['level']
        });
    }
    // 保存
    public btnSave() {
        const params = {};
        if (this.groupShow && (this.trapForm.value.group !== this.renderData['groupName'])) {
            params['CommunityName'] = this.trapForm.value.group;
        }
        if (this.dominShow && (this.trapForm.value.trapDomainSel.id !== this.renderData['domin'].id)) {
            params['ServerIdentity'] = this.trapForm.value.trapDomainSel.id;
        }
        if (this.trapForm.value.v3UserSel && (this.trapForm.value.v3UserSel.id !== this.v3Info)) {
            params['SNMPv3User'] = this.trapForm.value.v3UserSel.id;
        }
        if (this.trapForm.value.trapEnable !== this.renderData['enable']) {
            params['Enabled'] = this.trapForm.value.trapEnable;
        }
        if (this.trapForm.value.trapVersionSel.id !== this.renderData['version'].id) {
            params['Version'] = this.trapForm.value.trapVersionSel.id;
        }
        if (this.trapForm.value.trapModeSel.id !== this.renderData['mode'].id) {
            params['Mode'] = this.trapForm.value.trapModeSel.id;
        }
        if (this.trapForm.value.emailLevel.id !== this.renderData['level'].id) {
            params['AlarmSeverity'] = this.trapForm.value.emailLevel.id;
        }
        this.loading.state.next(true);
        this.service.trapSave(params).subscribe((res) => {
            const response = res['body'];
            this.btnSaveState = true;
            this.loading.state.next(false);
            if (response.error) {
                const errorId = getMessageId(response.error)[0].errorId;
                const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            } else {
                this.renderData = {
                    enable: response.Enabled,
                    version: this._filter(response.Version, this.trapVersion.list),
                    mode: this._filter(response.Mode, this.trapMode.list),
                    domin: this._filter(response.ServerIdentity, this.trapDomain.list),
                    groupName: '',
                    groupReName: '',
                    level: this._filter(response.AlarmSeverity, this.sendEmailLevel.list),
                };
                this.v3Info = response.SNMPv3User;
                for (const item of this.v3User.list) {
                    if (item.id === response.SNMPv3User) {
                        this.trapForm.patchValue({
                            v3UserSel: item
                        });
                    }
                }
                this.trapForm.patchValue({
                    group: '',
                    confirmGroup: '',
                });
                this.setTrapTable(response.Mode, response.TrapServerList);
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            }
        }, (error) => {
            this.loading.state.next(false);
            this.btnSaveState = true;
            const errorId = getMessageId(error.error)[0].errorId;
            const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
        });
    }

    // 取消修改
    public trapCancelBtn() {
        this.trapCancel = true;
        this.btnSaveState = true;
        this.getTrapInfo();
    }
    // 表格数据变化
    public tableChange(event) {
        if (event.key === 'save') {
            this.setTrapTable(event.value.TrapMode, event.value.TrapServer);
        }
    }
    /**
     * 根据id匹配对应的项
     * @param id arr中对应的id
     * @param arr 待匹配的数组
     * @returns 匹配到结果返回对应的对象。否则返回空对象
     */
    private _filter(id, arr) {
        for (const item of arr) {
            if (item['id'] === id) {
                return item;
            }
        }
        return {};
    }
    private _extendV3(v3List) {
        const tmpArr = [];
        v3List.forEach(item => {
            tmpArr.push({
                key: item,
                id: item,
            });
        });
        return tmpArr;
    }
}
