import { Component, OnInit } from '@angular/core';
import { UntypedFormBuilder, Validators } from '@angular/forms';
import { TranslateService } from '@ngx-translate/core';
import { TiValidators, TiValidationConfig } from '@cloud/tiny3';
import { SnmpService } from './services';
import { AlertMessageService, LoadingService, UserInfoService, ErrortipService } from 'src/app/common-app/service';
import { Router } from '@angular/router';
import { getBrowserType, getMessageId } from 'src/app/common-app/utils';
import { MultVaild } from 'src/app/common-app/utils/multValid';
import * as utils from 'src/app/common-app/utils';

@Component({
    selector: 'app-snmp',
    templateUrl: './snmp.component.html',
    styleUrls: ['../service.component.scss', './snmp.component.scss']
    })
export class SnmpComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private fb: UntypedFormBuilder,
        private snmpService: SnmpService,
        private alert: AlertMessageService,
        private errorTipService: ErrortipService,
        private router: Router,
        private user: UserInfoService,
        private loadingService: LoadingService
    ) { }

    // 权限判断
    public isConfigureUser = this.user.hasPrivileges(['ConfigureUsers']);
    public isSystemLock = this.user.systemLockDownEnabled;
    public loginRuleDisabled: boolean = false;
    public snmpReadWriteValid: boolean = true;
    public snmpReadOnlyValid: boolean = true;
    public browser: string = getBrowserType().browser;

    public snmpDisabled: boolean = true;
    public loginChange: boolean = false;
    public communitys = {
        readOnly: Symbol(),
        readWrite: Symbol()
    };
    public snmpDataView = {
        snmpEnable: {
            change: () => {
                this.protocolEnabledEN();
            }
        },
        snmpPort: {
            change: () => {
                this.protocolEnabledEN();
            }
        },
        productValue: {
            change: () => {
                this.protocolEnabledEN();
            }
        },
        locationValue: {
            change: () => {
                this.protocolEnabledEN();
            }
        },
        snmpList: {
            change: () => {
                this.protocolEnabledEN();
                if (this.snmpViewForm.root.get('sNMPv1Checked').value || this.snmpViewForm.root.get('sNMPv2Checked').value) {
                    this.showAlert['openAlert'] = true;
                    this.showAlert['type'] = 'warn';
                    this.showAlert['label'] = this.translate.instant('SERVICE_REDUCE_SECURITY');
                } else {
                    this.showAlert['openAlert'] = false;
                }
            }
        },
        longPwd: {
            change: () => {
                this.snmpViewForm.patchValue({
                    readOnlyControl: '',
                    readConfirmControl: '',
                    readWriteControl: '',
                    writeConfirmControl: ''
                });
                this.getPwdCode(this.snmpData);
                this.protocolEnabledEN();
            }
        },
        deleteReadOnly: {
            change: (event) => {
                this.deleteCommunity(event, this.communitys.readOnly);
                this.protocolEnabledEN();
            }
        },
        readOnly: {
            change: () => {
                this.snmpViewForm.get('readConfirmControl').updateValueAndValidity();
                this.snmpViewForm.get('readWriteControl').updateValueAndValidity();
                this.snmpReadWriteValid = this.snmpViewForm.get('readWriteControl').valid;
                this.protocolEnabledEN();
            }
        },
        readConfirm: {
            change: () => {
                this.snmpViewForm.get('readOnlyControl').updateValueAndValidity();
                this.protocolEnabledEN();
            }
        },
        deleteReadWrite: {
            change: (event) => {
                this.deleteCommunity(event, this.communitys.readWrite);
                this.protocolEnabledEN();
            }
        },
        readWrite: {
            change: () => {
                this.snmpViewForm.get('writeConfirmControl').updateValueAndValidity();
                this.snmpViewForm.get('readOnlyControl').updateValueAndValidity();
                this.snmpReadOnlyValid = this.snmpViewForm.get('readOnlyControl').valid;
                this.protocolEnabledEN();
            }
        },
        writeConfirm: {
            change: () => {
                this.snmpViewForm.get('readWriteControl').updateValueAndValidity();
                this.protocolEnabledEN();
            }
        },
        snmpConfigObj: {
            value: '',
            loginRuleList: []
        },
        snmpv3: {
            change: () => {
                this.protocolEnabledEN();
            }
        },
        snmpV3EngineValue: ''
    };

    public loginRuleCheckedArray = [];
    public snmpvCheckedArray = [];
    public snmpViewForm = this.fb.group({
        enableSwitchState: [{ value: false, disabled: false }, []],
        snmpPortValue: [
            { value: null, disabled: false },
            [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 65535)]
        ],
        productValueControl: [
            { value: null, disabled: false },
            [Validators.pattern(/^[a-zA-Z0-9~`!\?,".:;\-_''\(\)\{\}\[\]\/<>@#\$%\^&\*\+\|\\=\s]{0,255}$/)]
        ],
        locationValueControl: [
            { value: null, disabled: false },
            [Validators.pattern(/^[a-zA-Z0-9~`!\?,".:;\-_''\(\)\{\}\[\]\/<>@#\$%\^&\*\+\|\\=\s]{0,255}$/)]
        ],
        sNMPv1Checked: [{ value: null, disabled: false }, []],
        sNMPv2Checked: [{ value: null, disabled: false }, []],
        longPwdSwitchState: [{ value: true, disabled: false }, []],
        deleteReadChecked: [{ value: false, disabled: false }, []],
        readOnlyControl: [{ value: '', disabled: false }, []],
        deleteWriteControl: [{ value: false, disabled: false }, []],
        readConfirmControl: [{ value: '', disabled: false }, [MultVaild.equalTo('readOnlyControl')]],
        readWriteControl: [{ value: '', disabled: false }, []],
        writeConfirmControl: [{ value: '', disabled: false }, [MultVaild.equalTo('readWriteControl')]],
        snmpv3Checked: [{ value: null, disabled: false }, []],
    });
    // 端口
    validationPort: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('SERVICE_PORT_RANGES'),
            number: this.translate.instant('SERVICE_PORT_RANGES'),
            integer: this.translate.instant('SERVICE_PORT_RANGES'),
            rangeValue: this.translate.instant('SERVICE_PORT_RANGES')
        }
    };
    // 联系人
    validationProduct: TiValidationConfig = {
        tip: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_TIP'),
        type: 'blur',
        errorMessage: {
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };
    // 位置
    validationLocation: TiValidationConfig = {
        tip: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_TIP'),
        type: 'blur',
        errorMessage: {
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };
    // 只读团体名
    public validationReadOnly: TiValidationConfig = {
        type: 'blur',
        tipPosition: 'right',
        errorMessage: {
            rangeSize: ''
        }
    };
    // 读写团体名
    public validationReadWrite: TiValidationConfig = {
        type: 'blur',
        tipPosition: 'right',
        errorMessage: {
            rangeSize: ''
        }
    };
    // 确认只读团体名
    public validationReadConfirm: TiValidationConfig = {
        type: 'blur',
        tipPosition: 'right',
        errorMessage: {
            equalTo: this.translate.instant('SERVICE_READ_ONLYS')
        }
    };
    // 确认读写团体名
    public validationWriteConfirm: TiValidationConfig = {
        type: 'blur',
        tipPosition: 'right',
        errorMessage: {
            equalTo: this.translate.instant('SERVICE_READ_WRITES')
        }
    };
    // 初始化属性
    public showAlert = {
        type: '',
        openAlert: false,
        label: ''
    };
    public snmpParams: object = {};
    public loginRuleData: object = {};
    public snmpData: object = {};
    public snmpProtocolData: object = {};
    public params: object = {};
    public vncLoginRuleList = [];

    // 团体名校验
    public getPwdCode(snmpData) {
        if (snmpData) {
            const checkRuleStrict = snmpData.SNMPv1v2.PasswordComplexityCheckEnabled;
            // 开启密码检查功能
            if (checkRuleStrict) {
                if (this.snmpViewForm.root.get('longPwdSwitchState').value) {
                    // 开启超长口令,开启密码检查功能
                    this.snmpViewForm.get('readOnlyControl').setValidators(
                        [TiValidators.rangeSize(16, 32), MultVaild.pattern(), MultVaild.special(), MultVaild.noEmpty(),
                            MultVaild.notEqualTo('readWriteControl')]
                    );
                    this.snmpViewForm.get('readWriteControl').setValidators(
                        [TiValidators.rangeSize(16, 32), MultVaild.pattern(), MultVaild.special(), MultVaild.noEmpty(),
                            MultVaild.notEqualTo('readOnlyControl')]
                    );
                    this.validationReadOnly = {
                        passwordConfig: {
                            validator: {
                                rule: 'validatorsNew',
                                params: {},
                                message: {
                                    rangeSize: utils.formatEntry(this.translate.instant('VALIDATOR_PWD_VALIDATOR1'), [16, 32]),
                                    pattern: this.translate.instant('COMMON_SPECIAL_CHARACTERS'),
                                    special: this.translate.instant('COMMON_ALLOWING_CHARACTERS'),
                                    noEmpty: this.translate.instant('COMMON_CANNOT_CONTAIN_SPACES')
                                }
                            }
                        },
                        errorMessage: {
                            rangeSize: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            special: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            noEmpty: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            notEqualTo: this.translate.instant('SNMP_READONLY_CONFIRM_TIP')
                        }
                    };
                    this.validationReadWrite = {
                        passwordConfig: {
                            validator: {
                                rule: 'validatorsNew',
                                params: {},
                                message: {
                                    rangeSize: utils.formatEntry(this.translate.instant('VALIDATOR_PWD_VALIDATOR1'), [16, 32]),
                                    pattern: this.translate.instant('COMMON_SPECIAL_CHARACTERS'),
                                    special: this.translate.instant('COMMON_ALLOWING_CHARACTERS'),
                                    noEmpty: this.translate.instant('COMMON_CANNOT_CONTAIN_SPACES')
                                }
                            }
                        },
                        errorMessage: {
                            rangeSize: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            special: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            noEmpty: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            notEqualTo: this.translate.instant('SNMP_READONLY_CONFIRM_TIP')
                        }
                    };
                } else {
                    // 关闭超长口令,开启密码检查功能
                    this.snmpViewForm.get('readOnlyControl').setValidators(
                        [TiValidators.rangeSize(8, 32), MultVaild.pattern(), MultVaild.special(), MultVaild.noEmpty(),
                            MultVaild.notEqualTo('readWriteControl')]
                    );
                    this.snmpViewForm.get('readWriteControl').setValidators(
                        [TiValidators.rangeSize(8, 32), MultVaild.pattern(), MultVaild.special(), MultVaild.noEmpty(),
                            MultVaild.notEqualTo('readOnlyControl')]
                    );
                    this.validationReadOnly = {
                        passwordConfig: {
                            validator: {
                                rule: 'validatorsNew',
                                params: {},
                                message: {
                                    rangeSize: utils.formatEntry(this.translate.instant('VALIDATOR_PWD_VALIDATOR1'), [8, 32]),
                                    pattern: this.translate.instant('COMMON_SPECIAL_CHARACTERS'),
                                    special: this.translate.instant('COMMON_ALLOWING_CHARACTERS'),
                                    noEmpty: this.translate.instant('COMMON_CANNOT_CONTAIN_SPACES')
                                }
                            }
                        },
                        errorMessage: {
                            rangeSize: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            special: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            noEmpty: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            notEqualTo: this.translate.instant('SNMP_READONLY_CONFIRM_TIP')
                        }
                    };
                    this.validationReadWrite = {
                        passwordConfig: {
                            validator: {
                                rule: 'validatorsNew',
                                params: {},
                                message: {
                                    rangeSize: utils.formatEntry(this.translate.instant('VALIDATOR_PWD_VALIDATOR1'), [8, 32]),
                                    pattern: this.translate.instant('COMMON_SPECIAL_CHARACTERS'),
                                    special: this.translate.instant('COMMON_ALLOWING_CHARACTERS'),
                                    noEmpty: this.translate.instant('COMMON_CANNOT_CONTAIN_SPACES')
                                }
                            }
                        },
                        errorMessage: {
                            rangeSize: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            special: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            noEmpty: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                            notEqualTo: this.translate.instant('SNMP_READONLY_CONFIRM_TIP')
                        }
                    };
                }
            } else {
                // 关闭密码检查功能
                if (this.snmpViewForm.root.get('longPwdSwitchState').value) {
                    // 开启超长口令
                    this.snmpViewForm.get('readOnlyControl').setValidators(
                        [TiValidators.rangeSize(16, 32), MultVaild.notEqualTo('readWriteControl')]
                    );
                    this.snmpViewForm.get('readWriteControl').setValidators(
                        [TiValidators.rangeSize(16, 32), MultVaild.notEqualTo('readOnlyControl')]
                    );
                    this.snmpViewForm.updateValueAndValidity({
                        onlySelf: true,
                    });
                    // 只读团体名
                    this.validationReadOnly = {
                        type: 'blur',
                        tipPosition: 'right',
                        tip: this.translate.instant('IBMC_SNMP_COMMUNITY_LONG_TIP'),
                        errorMessage: {
                            rangeSize: this.translate.instant('SERVICE_LENGTH_EXCEPT_SPACES'),
                            notEqualTo: this.translate.instant('SNMP_READONLY_CONFIRM_TIP')
                        }
                    };
                    // 读写团体名
                    this.validationReadWrite = {
                        type: 'blur',
                        tipPosition: 'right',
                        tip: this.translate.instant('IBMC_SNMP_COMMUNITY_LONG_TIP'),
                        errorMessage: {
                            rangeSize: this.translate.instant('SERVICE_LENGTH_EXCEPT_SPACES'),
                            notEqualTo: this.translate.instant('SNMP_READONLY_CONFIRM_TIP')
                        }
                    };
                } else {
                    // 关闭超长口令
                    this.snmpViewForm.get('readOnlyControl').setValidators(
                        [TiValidators.rangeSize(1, 32), MultVaild.notEqualTo('readWriteControl')]
                    );
                    this.snmpViewForm.get('readWriteControl').setValidators(
                        [TiValidators.rangeSize(1, 32), MultVaild.notEqualTo('readOnlyControl')]
                    );
                    this.snmpViewForm.updateValueAndValidity({
                        onlySelf: true,
                    });
                    // 只读团体名
                    this.validationReadOnly = {
                        type: 'blur',
                        tipPosition: 'right',
                        tip: this.translate.instant('IBMC_SNMP_COMMUNITY_TIP'),
                        errorMessage: {
                            rangeSize: this.translate.instant('SERVICE_OFF_LENGTH_SPACES'),
                            notEqualTo: this.translate.instant('SNMP_READONLY_CONFIRM_TIP')
                        }
                    };
                    // 读写团体名
                    this.validationReadWrite = {
                        type: 'blur',
                        tipPosition: 'right',
                        tip: this.translate.instant('IBMC_SNMP_COMMUNITY_TIP'),
                        errorMessage: {
                            rangeSize: this.translate.instant('SERVICE_OFF_LENGTH_SPACES'),
                            notEqualTo: this.translate.instant('SNMP_READONLY_CONFIRM_TIP')
                        }
                    };
                }
            }
        }
    }

    public initData = (snmpData) => {
        // snmp使能
        this.snmpParams['enabbleState'] = snmpData.Enabled;
        // 端口
        this.snmpParams['snmpPort'] = snmpData.Port;
        // 联系人
        this.snmpParams['productValue'] = snmpData.SystemContact;
        // 位置
        this.snmpParams['locationValue'] = snmpData.SystemLocation;
        // SNMPv1 SNMPv2
        this.snmpParams['snmpV1Enabled'] = snmpData.SNMPv1v2.SNMPv1Enabled;
        this.snmpParams['snmpV2Enabled'] = snmpData.SNMPv1v2.SNMPv2Enabled;
        // 超长口令
        this.snmpParams['longPwdState'] = snmpData.SNMPv1v2.LongPasswordEnabled;
        // SNMPv3
        this.snmpParams['snmpV3Enabled'] = snmpData.SNMPv3.Enabled;

        this.snmpViewForm.patchValue({
            enableSwitchState: this.snmpParams['enabbleState'],
            snmpPortValue: this.snmpParams['snmpPort'],
            productValueControl: this.snmpParams['productValue'],
            locationValueControl: this.snmpParams['locationValue'],
            sNMPv1Checked: this.snmpParams['snmpV1Enabled'],
            sNMPv2Checked: this.snmpParams['snmpV2Enabled'],
            longPwdSwitchState: this.snmpParams['longPwdState'],
            snmpv3Checked: this.snmpParams['snmpV3Enabled'],
            deleteReadChecked: false,
            readOnlyControl: '',
            readConfirmControl: '',
            deleteWriteControl: false,
            readWriteControl: '',
            writeConfirmControl: ''
        });
        if (this.snmpViewForm.root.get('sNMPv1Checked').value || this.snmpViewForm.root.get('sNMPv2Checked').value) {
            this.showAlert['openAlert'] = true;
            this.showAlert['type'] = 'warn';
            this.showAlert['label'] = this.translate.instant('SERVICE_REDUCE_SECURITY');
        } else {
            this.showAlert['openAlert'] = false;
        }

        this.snmpDataView.snmpV3EngineValue = snmpData.SNMPv3.EngineID;

        // 登录规则
        this.snmpParams['vncLoginRuleList0'] = false;
        this.snmpParams['vncLoginRuleList1'] = false;
        this.snmpParams['vncLoginRuleList2'] = false;
        if (snmpData.SNMPv1v2.LoginRule.length > 0) {
            snmpData.SNMPv1v2.LoginRule.forEach((item) => {
                if (item === 'Rule1') {
                    this.snmpParams['vncLoginRuleList0'] = true;
                }
                if (item === 'Rule2') {
                    this.snmpParams['vncLoginRuleList1'] = true;
                }
                if (item === 'Rule3') {
                    this.snmpParams['vncLoginRuleList2'] = true;
                }
            });
        }
        this.ruleInit(snmpData);
        this.getPwdCode(snmpData);

        TiValidators.check(this.snmpViewForm);
    }
    // 登录规则初始化
    public ruleInit(snmpData): void {
        const loginRules = snmpData.SNMPv1v2.LoginRuleInfo;
        const temp = [];
        if (loginRules) {
            loginRules.forEach((item, i) => {
                temp.push(this.combination(i, item));
            });
            this.snmpDataView.snmpConfigObj['loginRuleList'] = temp;
            this.loginRuleCheckedArray = [];
            this.vncLoginRuleList = [];
            if (this.snmpParams['vncLoginRuleList0']) {
                this.loginRuleCheckedArray.push(this.snmpDataView.snmpConfigObj['loginRuleList'][0]);
                this.vncLoginRuleList.push(this.snmpDataView.snmpConfigObj['loginRuleList'][0]);
            }
            if (this.snmpParams['vncLoginRuleList1']) {
                this.loginRuleCheckedArray.push(this.snmpDataView.snmpConfigObj['loginRuleList'][1]);
                this.vncLoginRuleList.push(this.snmpDataView.snmpConfigObj['loginRuleList'][1]);
            }
            if (this.snmpParams['vncLoginRuleList2']) {
                this.loginRuleCheckedArray.push(this.snmpDataView.snmpConfigObj['loginRuleList'][2]);
                this.vncLoginRuleList.push(this.snmpDataView.snmpConfigObj['loginRuleList'][2]);
            }
        }
    }
    // 登录规则数据组装
    public combination(i, item): object {
        return {
            disabled: !item.Enabled,
            id: item.ID,
            descIPMac: true,
            title: item.ID.replace('Rule', this.translate.instant('SERVICE_SNMP_RULE')),
            descStartTime: item.StartTime === null ? '- - -' : item.StartTime,
            descEndTime: item.EndTime === null ? '- - -' : item.EndTime,
            descIP: item.IP === null ? '- - -' : item.IP,
            descMAC: item.Mac === null ? '- - -' : item.Mac
        };
    }
    public snmpGetInit = () => {
        this.snmpService.getSnmp().subscribe((response) => {
            this.snmpData = response['body'];
            this.initData(this.snmpData);
        }, () => {
            this.loadingService.state.next(false);
        }, () => {
            this.loadingService.state.next(false);
        });
    }
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
            this.snmpAuthority();
        }
    }
    // 权限
    public snmpAuthority() {
        // 具有用户配置权限可以设置SNMP使能、端口、可以设置SNMPV1、V2C、V3信息
        if (this.isConfigureUser && !this.isSystemLock) {
            this.snmpViewForm.root.get('enableSwitchState').enable();
            this.snmpViewForm.root.get('snmpPortValue').enable();
            this.snmpViewForm.root.get('productValueControl').enable();
            this.snmpViewForm.root.get('locationValueControl').enable();
            this.snmpViewForm.root.get('sNMPv1Checked').enable();
            this.snmpViewForm.root.get('sNMPv2Checked').enable();
            this.snmpViewForm.root.get('longPwdSwitchState').enable();
            this.snmpViewForm.root.get('deleteReadChecked').enable();
            this.snmpViewForm.root.get('readOnlyControl').enable();
            this.snmpViewForm.root.get('readConfirmControl').enable();
            this.snmpViewForm.root.get('deleteWriteControl').enable();
            this.snmpViewForm.root.get('readWriteControl').enable();
            this.snmpViewForm.root.get('writeConfirmControl').enable();
            this.snmpViewForm.root.get('snmpv3Checked').enable();
            this.loginRuleDisabled = false;

        } else {
            this.snmpViewForm.root.get('enableSwitchState').disable();
            this.snmpViewForm.root.get('snmpPortValue').disable();
            this.snmpViewForm.root.get('productValueControl').disable();
            this.snmpViewForm.root.get('locationValueControl').disable();
            this.snmpViewForm.root.get('sNMPv1Checked').disable();
            this.snmpViewForm.root.get('sNMPv2Checked').disable();
            this.snmpViewForm.root.get('longPwdSwitchState').disable();
            this.snmpViewForm.root.get('deleteReadChecked').disable();
            this.snmpViewForm.root.get('readOnlyControl').disable();
            this.snmpViewForm.root.get('readConfirmControl').disable();
            this.snmpViewForm.root.get('deleteWriteControl').disable();
            this.snmpViewForm.root.get('readWriteControl').disable();
            this.snmpViewForm.root.get('writeConfirmControl').disable();
            this.snmpViewForm.root.get('snmpv3Checked').disable();
            this.loginRuleDisabled = true;
        }
    }
    ngOnInit(): void {
        this.loadingService.state.next(true);
        this.snmpAuthority();

        this.snmpGetInit();
    }

    // 数据是否发生变化
    public protocolEnabledEN = () => {
        if (!this.snmpViewForm.valid) {
            this.snmpDisabled = true;
            return;
        }
        this.loginChange = false;
        this.params = {
            SNMPv1v2: {},
            SNMPv3: {}
        };
        if (!this.snmpParams) {
            return;
        }
        if (this.snmpViewForm.root.get('enableSwitchState').value !== this.snmpParams['enabbleState']) {
            this.params['Enabled'] = this.snmpViewForm.root.get('enableSwitchState').value;
        }
        if (parseInt(this.snmpViewForm.root.get('snmpPortValue').value, 10)  !== this.snmpParams['snmpPort']) {
            this.params['Port'] = parseInt(this.snmpViewForm.root.get('snmpPortValue').value, 10);
        }
        if (this.snmpViewForm.root.get('productValueControl').value !== this.snmpParams['productValue']) {
            this.params['SystemContact'] = this.snmpViewForm.root.get('productValueControl').value;
        }
        if (this.snmpViewForm.root.get('locationValueControl').value !== this.snmpParams['locationValue']) {
            this.params['SystemLocation'] = this.snmpViewForm.root.get('locationValueControl').value;
        }
        if (this.snmpViewForm.root.get('sNMPv1Checked').value !== this.snmpParams['snmpV1Enabled']) {
            this.params['SNMPv1v2'].SNMPv1Enabled = this.snmpViewForm.root.get('sNMPv1Checked').value;
        }
        if (this.snmpViewForm.root.get('sNMPv2Checked').value !== this.snmpParams['snmpV2Enabled']) {
            this.params['SNMPv1v2'].SNMPv2Enabled = this.snmpViewForm.root.get('sNMPv2Checked').value;
        }
        if (this.snmpViewForm.root.get('longPwdSwitchState').value !== this.snmpParams['longPwdState']) {
            this.params['SNMPv1v2'].LongPasswordEnabled = this.snmpViewForm.root.get('longPwdSwitchState').value;
        }
        if (this.snmpViewForm.root.get('snmpv3Checked').value !== this.snmpParams['snmpV3Enabled']) {
            this.params['SNMPv3'].Enabled = this.snmpViewForm.root.get('snmpv3Checked').value;
        }
        if (this.snmpViewForm.root.get('readOnlyControl').value !== '' && this.snmpViewForm.root.get('readOnlyControl').value !== null) {
            this.params['SNMPv1v2'].ROCommunityName = this.snmpViewForm.root.get('readOnlyControl').value;
        }
        if (this.snmpViewForm.root.get('readWriteControl').value !== '' && this.snmpViewForm.root.get('readWriteControl').value !== null) {
            this.params['SNMPv1v2'].RWCommunityName = this.snmpViewForm.root.get('readWriteControl').value;
        }
        if (this.snmpViewForm.value.deleteReadChecked) {
            this.params['SNMPv1v2'].ROCommunityName = '';
        }
        if (this.snmpViewForm.value.deleteWriteControl) {
            this.params['SNMPv1v2'].RWCommunityName = '';
        }
        // 登录规则
        const loginRules = [];
        const loginRuleStart = [];
        this.loginRuleCheckedArray.forEach((item, i) => {
            if (item.id) {
                loginRules.push(item.id);
            }
        });
        this.vncLoginRuleList.forEach((item, i) => {
            if (item.id) {
                loginRuleStart.push(item.id);
            }
        });
        if (loginRules.length !== loginRuleStart.length) {
            this.params['SNMPv1v2'].LoginRule = loginRules;
            this.loginChange = true;
        } else {
            for (const key of loginRules) {
                if (!loginRuleStart.includes(key)) {
                    this.params['SNMPv1v2'].LoginRule = loginRules;
                    this.loginChange = true;
                    break;
                }
            }
        }
        this.deleteNULL(this.params);
        if (JSON.stringify(this.params) !== '{}') {
            this.snmpDisabled = false;
        } else {
            this.snmpDisabled = true;
        }
    }
    // 恢复默认值
    public restFN(): void {
        this.snmpViewForm.patchValue({ snmpPortValue: 161 });
        this.protocolEnabledEN();
    }
    // 点击跳转安全配置页面
    public configRule(): void {
        if (this.isConfigureUser) {
            sessionStorage.setItem('securityTab', 'loginRule');
            this.router.navigate(['/navigate/user/security']);
        } else {
            return;
        }
    }
    // 登录规则
    public checkbox01FN(item): void {
        this.protocolEnabledEN();
    }
    // 保存
    public snmpSaveButton(): void {
        this.snmpDisabled = true;
        this.protocolEnabledEN();
        this.snmpService.setSnmpService(this.params).subscribe((response) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.snmpData = response['body'];
            this.initData(this.snmpData);
        }, (error) => {
            const errorId = getMessageId(error.error)[0].errorId;
            const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({type: 'error', label: errorMessage});
            this.snmpData = error.error.data;
            this.initData(this.snmpData);
        });
    }
    /**
     * 过滤掉对象中的空值
     * @param obj 要过滤的对象集合
     * @returns 过滤后的对象
     */
	public deleteNULL(obj: object) {
		for (const key in obj) {
			if (
				Object.prototype.hasOwnProperty.call(obj, key)
				&& (JSON.stringify(obj[key]) === '{}')
			) {
				delete obj[key];
			}
		}
		return obj;
	}

    public deleteCommunity(event, type): void {
        if (event) {
            if (type === this.communitys.readOnly) {
                this.snmpViewForm.root.get('readOnlyControl').disable();
                this.snmpViewForm.root.get('readConfirmControl').disable();
                this.snmpViewForm.patchValue({
                    readOnlyControl: '',
                    readConfirmControl: ''
                });
            } else {
                this.snmpViewForm.root.get('readWriteControl').disable();
                this.snmpViewForm.root.get('writeConfirmControl').disable();
                this.snmpViewForm.patchValue({
                    readWriteControl: '',
                    writeConfirmControl: ''
                });
            }
        } else {
            if (type === this.communitys.readOnly) {
                this.snmpViewForm.root.get('readOnlyControl').enable();
                this.snmpViewForm.root.get('readConfirmControl').enable();
            } else {
                this.snmpViewForm.root.get('readWriteControl').enable();
                this.snmpViewForm.root.get('writeConfirmControl').enable();
            }
        }
    }
}
