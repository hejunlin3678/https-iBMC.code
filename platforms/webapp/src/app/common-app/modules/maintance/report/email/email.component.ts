import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormControl, Validators } from '@angular/forms';
import { TiValidators, TiValidationConfig, TiTableColumns, TiTableSrcData, TiMessageService, TiFileInfo, TiFileItem } from '@cloud/tiny3';
import { EmailService } from './service';
import { Smtp } from './model';
import { getMessageId, DESCRIPTION, MultVaild } from 'src/app/common-app/utils';
import { SerAddrValidators } from 'src/app/common-app/utils/valid';
import { VALID_USERNAME, VALID_USERNAME_REQUIRE } from './email.utils';
import { AlertMessageService, UserInfoService, LoadingService, ErrortipService, SystemLockService, TimeoutService } from 'src/app/common-app/service';
@Component({
    selector: 'app-email',
    templateUrl: './email.component.html',
    styleUrls: ['./email.component.scss']
})
export class EmailComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private service: EmailService,
        private alert: AlertMessageService,
        private loading: LoadingService,
        private user: UserInfoService,
        private errorTip: ErrortipService,
        private tiMessage: TiMessageService,
        private lockService: SystemLockService,
        private timeoutService: TimeoutService,
    ) { }
    public isSystemLock: boolean = this.user.systemLockDownEnabled;
    public hasPermission = this.user.hasPrivileges(['OemSecurityMgmt']);
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public emailDisabled: boolean = true;
    public isUserRequre: boolean = false;
    public isAddressRequire: boolean = false;
    public isEmailCancel: boolean = false;
    public showSmtpCertificate: boolean = false;

    public smtp = {
        label: 'ALARM_REPORT_SMTP_ENABLE',
        state: true,
        change: () => {
            this.addressRequire();
            this.changeSaveStatus();
        }
    };
    public serviceAddr = {
        label: 'ALARM_REPORT_SMTP_ADDR',
        value: '',
        change: () => {
            this.changeSaveStatus();
        }
    };
    public servicePort = {
        label: 'ALARM_REPORT_SMTP_PORT',
        value: '',
        change: () => {
            this.changeSaveStatus();
        }
    };
    // 服务器地址校验
    public validationAddr: TiValidationConfig = {
        type: 'blur',
        tip: this.translate.instant('COMMON_IPV4_IPV6_DOMIN'),
        tipPosition: 'right',
        errorMessage: {
            pattern: this.translate.instant('COMMON_FORMAT_ERROR')
        }
    };
    // 服务器端口校验
    public validationPort: TiValidationConfig = {
        type: 'blur',
        tipPosition: 'right',
        errorMessage: {
            required: this.translate.instant('SERVICE_PORT_RANGES'),
            number: this.translate.instant('SERVICE_PORT_RANGES'),
            integer: this.translate.instant('SERVICE_PORT_RANGES'),
            rangeValue: this.translate.instant('SERVICE_PORT_RANGES')
        }
    };
    public addrControl: UntypedFormControl = new UntypedFormControl(this.serviceAddr.value, [SerAddrValidators.validateIp46AndDomin()]);
    public portControl: UntypedFormControl = new UntypedFormControl(this.servicePort.value,
        [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 65535)]);
    public tls = {
        label: 'ALARM_REPORT_TLS_ENABLE',
        list: [{
            id: true,
            key: 'COMMON_YES',
            tlsId: 'tlsOk'
        }, {
            id: false,
            key: 'COMMON_NO',
            tlsId: 'tlsNo'
        }],
        value: false,
        change: () => {
            this.showAlert['openAlert'] = !this.tls.value;
            this.changeSaveStatus();
        }
    };
    public smtpCertificate = {
        label: 'ALARM_VERIFYING_TERMINA_CERTIFICATE',
        state: true,
        change: () => {
            this.changeSaveStatus();
        }
    };
    // 上传smtp端证书
    public smtpUpload = {
        url: 'UI/Rest/Maintenance/EmailNotification/ImportRootCertificate',
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
     // smtp证书
     public smtpCertificateInfo = {
        issuer: '',
        subject: '',
        validNotBefore: '',
        validNotAfter: '',
        serialNumber: '',
    };
    public showAlert = {
        type: 'warn',
        openAlert: false,
        label: this.translate.instant('ALARM_REPORT_EMAIL_TLS')
    };
    public emailInfo = {
        label: 'ALARM_REPORT_IS_ANONYMOUS',
        list: [{
            id: true,
            key: 'COMMON_YES',
            emailInfoId: 'infoOk'
        }, {
            id: false,
            key: 'COMMON_NO',
            emailInfoId: 'infoNo'
        }],
        value: false,
        change: (state) => {
            if (state) {
                this.userNameControl.disable();
                this.userPwdControl.disable();
            } else {
                this.userNameControl.enable();
                this.userPwdControl.enable();
            }
            this.isSmtpOpen();
            this.changeSaveStatus();
        }
    };
    public sendEmailUserName = {
        label: 'ALARM_REPORT_SEND_USERNAME',
        value: '',
        change: () => {
            this.changeSaveStatus();
        }
    };
    // 用户名校验
    public validationUserName: TiValidationConfig = {
        type: 'blur',
        tip: this.translate.instant('ALARM_REPORT_EMAIL_USERNAME_TIP'),
        tipPosition: 'right',
        errorMessage: {
            pattern: this.translate.instant('COMMON_FORMAT_ERROR'),
            notAllWhiteSpace: this.translate.instant('COMMON_FORMAT_ERROR')
        }
    };
    public userNameControl: UntypedFormControl = new UntypedFormControl(this.sendEmailUserName.value, [Validators.pattern(VALID_USERNAME)]);
    public sendEmailUserPwd = {
        label: 'ALARM_REPORT_SEND_PWD',
        value: '',
        change: () => {
            this.changeSaveStatus();
        }
    };
    // 密码校验
    public validationUserPwd: TiValidationConfig = {
        type: 'blur',
        tipPosition: 'right',
        errorMessage: {
            rangeSize: this.translate.instant('COMMON_FORMAT_ERROR')
        }
    };
    public userPwdControl: UntypedFormControl = new UntypedFormControl(this.sendEmailUserPwd.value, [TiValidators.rangeSize(0, 50)]);
    public sendEmailAddr = {
        label: 'ALARM_REPORT_SEND_ADDR',
        value: '',
        change: () => {
            this.changeSaveStatus();
        }
    };
    // 发送邮件验证
    public validationSendEmail: TiValidationConfig = {
        type: 'blur',
        tip: this.translate.instant('ALARM_FULL_EMAIL_TIP'),
        tipPosition: 'right',
        errorMessage: {
            pattern: this.translate.instant('COMMON_FORMAT_ERROR'),
            emailValidator: this.translate.instant('COMMON_FORMAT_ERROR'),
        }
    };
    public sendEmailControl: UntypedFormControl = new UntypedFormControl(this.sendEmailAddr.value,
        [MultVaild.emailValidator(), Validators.pattern(DESCRIPTION)]);
    public sendEmailSubject = {
        label: 'ALARM_REPORT_EMAIL_SUBJECT',
        value: '',
        change: () => {
            this.changeSaveStatus();
        }
    };
    // 主题类型校验
    public validationSubject: TiValidationConfig = {
        type: 'blur',
        tip: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_TIP'),
        tipPosition: 'right',
        errorMessage: {
            rangeSize: this.translate.instant('COMMON_FORMAT_ERROR'),
            minCharType: this.translate.instant('COMMON_FORMAT_ERROR')
        }
    };
    public subjectControl: UntypedFormControl = new UntypedFormControl(
        this.sendEmailSubject.value,
        [TiValidators.rangeSize(0, 255), TiValidators.minCharType(1)]
    );
    public sendEmailSubjectAppend = {
        label: 'ALARM_REPORT_EMAIL_SUBJECT_APPEND',
        value: '',
        list: [{
            id: 'BoardSN',
            key: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_1'),
            checked: false
        }, {
            id: 'ProductAssetTag',
            key: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_2'),
            checked: false
        }, {
            id: 'HostName',
            key: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_3'),
            checked: false
        }],
        change: () => {
            this.changeSaveStatus();
        }
    };
    public sendEmailLevel = {
        label: 'ALARM_REPORT_EMAIL_ALARM_LEVEL',
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
        change: () => {
            this.changeSaveStatus();
        }
    };
    public sendEmailLevelSelected = this.sendEmailLevel.list[0];
    public initDefaultData: object = {
        smtpEnable: false,
        smtpCertificateEnable: false,
        smtpServiceAddr: '',
        smtpServicePort: 0,
        tlsEnable: false,
        emailAnonymous: false,
        userName: '',
        userPwd: '',
        emailAddress: '',
        emailSubject: '',
        emailSubjectAppend: { list: [] },
        alarmLevel: { id: '', key: '' }
    };
    public defaultData: Smtp = new Smtp(this.initDefaultData);
    // 表格
    public columns: TiTableColumns = [
        {
            title: 'EVENT_ORDER',
            width: '5%'
        },
        {
            title: 'ALARM_REPORT_EMAIL_ADDRESS',
            width: '20%'
        },
        {
            title: 'COMMON_DESCRIPTION',
            width: '15%'
        },
        {
            title: 'ALARM_REPORT_EMAIL_SEND',
            width: '20%'
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
    public tempTableData = [];

    public copy(obj) {
        return Object.assign({}, obj);
    }
    ngOnInit(): void {
        this.init();
        // 防止切换页签时禁用会慢几秒
        if (this.isSystemLock || !this.isConfigureComponentsUser) {
            this.addrControl.disable();
            this.portControl.disable();
            this.userNameControl.disable();
            this.userPwdControl.disable();
            this.sendEmailControl.disable();
            this.subjectControl.disable();
        }
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.isSystemLock !== res) {
                this.isSystemLock = res;
                if (this.isSystemLock || !this.isConfigureComponentsUser) {
                    this.addrControl.disable();
                    this.portControl.disable();
                    this.userNameControl.disable();
                    this.userPwdControl.disable();
                    this.sendEmailControl.disable();
                    this.subjectControl.disable();
                } else {
                    this.addrControl.enable();
                    this.portControl.enable();
                    this.sendEmailControl.enable();
                    this.subjectControl.enable();
                    if (!this.emailInfo.value) {
                        this.userNameControl.enable();
                        this.userPwdControl.enable();
                    }
                }
            }
        });
    }

    public init() {
        this.loading.state.next(true);
        this.service.getEmailInfo().subscribe((res) => {
            const response = res['body'];
            this.smtp.state = response.Enabled ? response.Enabled : false;
            // 校验端服务器证书状态开关值
            this.smtpCertificate.state = response.CertVerificationEnabled;
            this.serviceAddr.value = response.ServerAddress ? response.ServerAddress : '';
            this.servicePort.value = response.ServerPort ? response.ServerPort : this.service.servicePort;
            this.tls.value = response.TLSEnabled;
            this.showAlert['openAlert'] = !this.tls.value;
            this.emailInfo.value = response.EmailInfo.AnonymousLoginEnabled;
            this.sendEmailUserPwd.value = response.EmailInfo.SenderPassword;
            if (response.EmailInfo.AnonymousLoginEnabled || this.isSystemLock || !this.isConfigureComponentsUser) {
                this.userNameControl.disable();
                this.userPwdControl.disable();
            } else {
                // 匿名不需要密码
                this.userNameControl.enable();
                this.userPwdControl.enable();
                this.sendEmailUserPwd.value = response.EmailInfo.SenderPassword ? response.EmailInfo.SenderPassword : '';
            }
            // 有端证书时处理证书信息显示内容
            if (response.RootCertificate) {
                this.smtpCertificateInfo.issuer = response.RootCertificate.Issuer;
                this.smtpCertificateInfo.subject = response.RootCertificate.Subject;
                this.smtpCertificateInfo.validNotBefore = response.RootCertificate.ValidNotBefore;
                this.smtpCertificateInfo.validNotAfter = response.RootCertificate.ValidNotAfter;
                this.smtpCertificateInfo.serialNumber = response.RootCertificate.SerialNumber;
                this.showSmtpCertificate = true;
            } else {
                this.showSmtpCertificate = false;
            }
            this.isSmtpOpen();
            this.addressRequire();
            this.sendEmailUserName.value = response.EmailInfo.SenderUserName ? response.EmailInfo.SenderUserName : '';
            this.sendEmailAddr.value = response.EmailInfo.SenderAddress ? response.EmailInfo.SenderAddress : '';
            this.sendEmailSubject.value = response.EmailInfo.EmailSubject;
            this.sendEmailLevelSelected = this._setAlarmLevel(response.EmailInfo.AlarmSeverity);
            this._setSubjectAppend(response.EmailInfo.EmailSubjectContains);
            this.defaultData = new Smtp({
                smtpEnable: this.smtp.state,
                smtpCertificateEnable: this.smtpCertificate.state,
                smtpServiceAddr: this.serviceAddr.value,
                smtpServicePort: parseInt(this.servicePort.value, 10),
                tlsEnable: this.tls.value,
                emailAnonymous: this.emailInfo.value,
                userName: this.sendEmailUserName.value,
                userPwd: this.sendEmailUserPwd.value,
                emailAddress: this.sendEmailAddr.value,
                emailSubject: this.sendEmailSubject.value,
                emailSubjectAppend: this.sendEmailSubjectAppend,
                alarmLevel: this.sendEmailLevelSelected
            });
            const arr = [];
            response.RecipientList.forEach((item) => {
                const obj = {};
                obj['isEdit'] = false;
                obj['num'] = Number(item.ID) + 1;
                obj['emailEnable'] = item.Enabled;
                obj['address'] = item.EmailAddress ? item.EmailAddress : '';
                obj['description'] = item.Description ? item.Description : '';
                obj['validationAddr'] = {
                    tip: this.translate.instant('ALARM_FULL_EMAIL_TIP'),
                    tipPosition: 'right',
                    errorMessage: {
                        pattern: this.translate.instant('COMMON_FORMAT_ERROR'),
                        emailValidator: this.translate.instant('COMMON_FORMAT_ERROR'),
                    }
                };
                obj['addrControl'] = new UntypedFormControl(obj['address'], [
                    MultVaild.emailValidator(), Validators.pattern(DESCRIPTION)]);
                obj['validationDesc'] = {
                    tip: this.translate.instant('ALARM_REPORT_EMAIL_SUBJECT_TIP'),
                    tipPosition: 'right',
                    errorMessage: {
                        pattern: this.translate.instant('COMMON_FORMAT_ERROR')
                    }
                };
                obj['descControl'] = new UntypedFormControl(obj['description'], [Validators.pattern(DESCRIPTION)]);
                arr.push(obj);
            });
            if (!this.isEmailCancel) {
                this.srcData.data = arr;
            }
            this.isEmailCancel = false;
            this.loading.state.next(false);
        }, () => {
            this.loading.state.next(false);
        });
    }
    private _setAlarmLevel(str: string) {
        for (const item of this.sendEmailLevel.list) {
            if (item.id === str) {
                return item;
            }
        }
    }
    private _setSubjectAppend(arr: []) {
        if (arr.length === 0) {
            return;
        }
        this.sendEmailSubjectAppend.list.forEach((item) => {
            for (const key of arr) {
                if (key === item.id) {
                    item.checked = true;
                }
            }
        });
    }
    // SMTP使能开启时增加服务器地址必填校验
    public addressRequire() {
        if (this.smtp.state) {
            this.isAddressRequire = true;
            this.addrControl.clearValidators();
            this.addrControl.setValidators([TiValidators.required, SerAddrValidators.validateIp46AndDomin()]);
            this.addrControl.markAsTouched();
            this.addrControl.updateValueAndValidity();
        } else {
            this.isAddressRequire = false;
            this.addrControl.clearValidators();
            this.addrControl.setValidators([SerAddrValidators.validateIp46AndDomin()]);
            this.addrControl.markAsTouched();
            this.addrControl.updateValueAndValidity();
        }
    }
    // 是否匿名
    public isSmtpOpen() {
        if (!this.emailInfo.value) {
            this.isUserRequre = true;
            this.validationUserName = {
                type: 'blur',
                tip: this.translate.instant('ALARM_REPORT_EMAIL_USERNAME_REQUIRE'),
                tipPosition: 'right',
                errorMessage: {
                    pattern: this.translate.instant('COMMON_FORMAT_ERROR'),
                    notAllWhiteSpace: this.translate.instant('COMMON_FORMAT_ERROR')
                }
            };
            this.userNameControl.clearValidators();
            this.userNameControl.setValidators([TiValidators.required, Validators.pattern(VALID_USERNAME_REQUIRE),
            MultVaild.notAllWhiteSpace()]);
            this.userNameControl.updateValueAndValidity();
        } else {
            this.isUserRequre = false;
            this.validationUserName = {
                type: 'blur',
                tip: this.translate.instant('ALARM_REPORT_EMAIL_USERNAME_TIP'),
                tipPosition: 'right',
                errorMessage: {
                    pattern: this.translate.instant('COMMON_FORMAT_ERROR'),
                    notAllWhiteSpace: this.translate.instant('COMMON_FORMAT_ERROR')
                }
            };
            this.userNameControl.clearValidators();
            this.userNameControl.setValidators([Validators.pattern(VALID_USERNAME)]);
            this.userNameControl.markAsTouched();
            this.userNameControl.updateValueAndValidity();
            this.userPwdControl.clearValidators();
            this.userPwdControl.setValidators([TiValidators.rangeSize(0, 50)]);
            this.userPwdControl.markAsTouched();
            this.userPwdControl.updateValueAndValidity();
        }
    }
    public changeSaveStatus() {
        const isValid = this.subjectControl.errors
            || this.sendEmailControl.errors
            || this.userPwdControl.errors
            || this.userNameControl.errors
            || this.addrControl.errors
            || this.portControl.errors;
        if (isValid) {
            this.emailDisabled = true;
            return;
        }
        const isEqul = this.smtp.state === this.defaultData['smtpEnable']
            && this.smtpCertificate.state === this.defaultData['smtpCertificateEnable']
            && this.serviceAddr.value === this.defaultData['smtpServiceAddr']
            && parseInt(this.servicePort.value, 10) === this.defaultData['smtpServicePort']
            && this.tls.value === this.defaultData['tlsEnable']
            && this.emailInfo.value === this.defaultData['emailAnonymous']
            && this.sendEmailUserName.value === this.defaultData['userName']
            && this.sendEmailUserPwd.value === this.defaultData['userPwd']
            && this.sendEmailAddr.value === this.defaultData['emailAddress']
            && this.sendEmailSubject.value === this.defaultData['emailSubject']
            && this._checkSubjectAppend() && this._checkAlarmEvent();
        if (isEqul) {
            this.emailDisabled = true;
        } else {
            this.emailDisabled = false;
        }
    }
    public tableChange(event) {
        switch (event.key) {
            case 'save':
                const isValid = event.value.addrControl.errors || event.value.descControl.errors;
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
                this.tempTableData[event.value.num - 1] = this.copy(event.value);
                event.value.isEdit = true;
                break;
            case 'test':
                // 若参数未保存，弹框提示。
                if (!this.emailDisabled) {
                    const emailTestModel = this.tiMessage.open({
                        id: 'emailTestModel',
                        type: 'prompt',
                        okButton: {
                            show: true,
                            autofocus: false,
                            click() {
                                emailTestModel.close();
                            }
                        },
                        cancelButton: {
                            show: false
                        },
                        title: this.translate.instant('COMMON_CONFIRM'),
                        content: this.translate.instant('ALARM_REPORT_EMAIL_TEST')
                    });
                } else {
                    this.test(event.value);
                }
                break;
            default: return;
        }
    }
    public save(data) {
        const obj = {
            ID: data.num - 1,
            Enabled: data.emailEnable,
            EmailAddress: data.address,
            Description: data.description
        };
        const params = {
            RecipientList: [obj]
        };
        this.loading.state.next(true);
        this.service.emailSave(params).subscribe((res) => {
            const response = res['body'];
            if (response?.error) {
                this.cancel(data);
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            } else {
                const resSingle = response.RecipientList[data.num - 1];
                data['isEdit'] = false;
                data['num'] = Number(resSingle.ID) + 1;
                data['emailEnable'] = resSingle.Enabled;
                data['address'] = resSingle.EmailAddress ? resSingle.EmailAddress : '';
                data['description'] = resSingle.Description ? resSingle.Description : '';
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            }
            this.loading.state.next(false);
        }, (error) => {
            this.cancel(data);
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            this.loading.state.next(false);
        });
    }
    public test(data) {
        const params = {
            ID: data.num - 1
        };
        this.loading.state.next(true);
        this.service.emailSendTest(params).subscribe(() => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.loading.state.next(false);
        }, (error) => {
            this.loading.state.next(false);
            const errorMessage = this.errorTip.getErrorMessage(getMessageId(error.error)[0].errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
        });
    }
    public cancel(data) {
        data.address = this.tempTableData[data.num - 1]['address'];
        data.description = this.tempTableData[data.num - 1]['description'];
        data.emailEnable = this.tempTableData[data.num - 1]['emailEnable'];
        this.tempTableData[data.num - 1] = null;
    }
    // 邮件保存
    public emailSave() {
        // 密码保存时触发校验
        const isPassword = !this.sendEmailUserPwd.value && this.isUserRequre;
        if (isPassword) {
            this.userPwdControl.clearValidators();
            this.userPwdControl.setValidators([TiValidators.required, TiValidators.rangeSize(1, 50)]);
            this.userPwdControl.markAsTouched();
            this.userPwdControl.updateValueAndValidity();
            return;
        }
        const params = {};
        const emailInfo = {};
        if (!this.emailInfo.value) {
            if (this.sendEmailUserName.value !== this.defaultData['userName']) {
                emailInfo['SenderUserName'] = this.sendEmailUserName.value;
            }
            if (this.sendEmailUserPwd.value !== this.defaultData['userPwd']) {
                emailInfo['SenderPassword'] = this.sendEmailUserPwd.value;
            }
        }
        if (this.smtp.state !== this.defaultData['smtpEnable']) {
            params['Enabled'] = this.smtp.state;
        }
        if (this.smtpCertificate.state !== this.defaultData['smtpCertificateEnable']) {
            params['CertVerificationEnabled'] = this.smtpCertificate.state;
        }
        if (this.serviceAddr.value !== this.defaultData['smtpServiceAddr']) {
            params['ServerAddress'] = this.serviceAddr.value;
        }
        if (parseInt(this.servicePort.value, 10) !== this.defaultData['smtpServicePort']) {
            params['ServerPort'] = parseInt(this.servicePort.value, 10);
        }
        if (this.tls.value !== this.defaultData['tlsEnable']) {
            params['TLSEnabled'] = this.tls.value;
        }
        if (this.emailInfo.value !== this.defaultData['emailAnonymous']) {
            emailInfo['AnonymousLoginEnabled'] = this.emailInfo.value;
        }
        if (this.sendEmailAddr.value !== this.defaultData['emailAddress']) {
            emailInfo['SenderAddress'] = this.sendEmailAddr.value;
        }
        if (this.sendEmailSubject.value !== this.defaultData['emailSubject']) {
            emailInfo['EmailSubject'] = this.sendEmailSubject.value;
        }
        if (!this._checkSubjectAppend()) {
            const arr = [];
            this.sendEmailSubjectAppend.list.forEach((item) => {
                if (item.checked) {
                    arr.push(item.id);
                }
            });
            emailInfo['EmailSubjectContains'] = arr;
        }
        if (!this._checkAlarmEvent()) {
            emailInfo['AlarmSeverity'] = this.sendEmailLevelSelected.id;
        }
        this.loading.state.next(true);
        if (JSON.stringify(emailInfo) !== '{}') {
            params['EmailInfo'] = emailInfo;
        }
        this.service.emailSave(params).subscribe((res) => {
            if (res['body'].error) {
                this.alert.eventEmit.emit({ type: 'error', label: 'USER_EDIT_PARTIALLY_FAILED' });
                this.loading.state.next(false);
                this.emailDisabled = false;
                return;
            }
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.emailDisabled = true;
            this.init();
            this.userPwdControl.clearValidators();
            this.userPwdControl.setValidators([TiValidators.rangeSize(0, 50)]);
            this.userPwdControl.markAsTouched();
            this.userPwdControl.updateValueAndValidity();
        }, (error) => {
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            this.loading.state.next(false);
            this.emailDisabled = false;
        });
    }
    // 修改取消
    public emailCancel() {
        this.emailDisabled = true;
        this.isEmailCancel = true;
        this.sendEmailSubjectAppend.list.forEach((item) => {
            item.checked = false;
        });
        this.userPwdControl.clearValidators();
        this.init();
    }
    private _checkSubjectAppend() {
        for (const item of this.defaultData['emailSubjectAppend'].list) {
            for (const key of this.sendEmailSubjectAppend.list) {
                if (item.id === key.id) {
                    if (item.checked !== key.checked) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    private _checkAlarmEvent() {
        if (this.defaultData['alarmLevel'].id !== this.sendEmailLevelSelected.id) {
            return false;
        }
        return true;
    }
}
