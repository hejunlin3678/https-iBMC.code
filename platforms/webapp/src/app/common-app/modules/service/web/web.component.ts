import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { Validators, UntypedFormBuilder } from '@angular/forms';
import { TiValidators, TiValidationConfig, TiModalService, TiMessageService, TiFileItem } from '@cloud/tiny3';
import { HttpWebService } from './services';
import { getMessageId } from 'src/app/common-app/utils';
import { CustomValidators } from './model';
import {
    UserInfoService,
    AlertMessageService,
    CommonService,
    LoadingService,
    ErrortipService,
    TimeoutService
} from 'src/app/common-app/service';

@Component({
    selector: 'app-web',
    templateUrl: './web.component.html',
    styleUrls: ['../service.component.scss', './web.component.scss'],
})
export class WebComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private httpWebService: HttpWebService,
        private tiModal: TiModalService,
        private tiMessage: TiMessageService,
        private user: UserInfoService,
        private alert: AlertMessageService,
        private commonService: CommonService,
        private errorTipService: ErrortipService,
        private loadingService: LoadingService,
        private fb: UntypedFormBuilder,
        private timeoutService: TimeoutService,
    ) { }
    public bfRadioChangeState: boolean = false;
    // 权限
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public isOemSecurityMgmtUser = this.user.hasPrivileges(['OemSecurityMgmt']);
    public isSystemLock = this.user.systemLockDownEnabled;

    // 初始化
    public webData = {};
    public sessionRes = {};
    public generateData = {};
    public dropDown: boolean = true;
    public isCollapsed: boolean = true;
    public dialogBox;
    public isPresetCert: boolean = !this.user.customeCertFlag;

    public webServiceForm = this.fb.group({
        httpEnableState: [{ value: true, disabled: false }, []],
        webHttpControl: [
            { value: null, disabled: false },
            [
                TiValidators.required,
                TiValidators.number,
                TiValidators.integer,
                TiValidators.rangeValue(1, 65535),
                CustomValidators.isEqualHTTPS('webHttpsControl')
            ]
        ],
        httpsEnableState: [{ value: true, disabled: false }, []],
        webHttpsControl: [
            { value: null, disabled: false },
            [
                TiValidators.required,
                TiValidators.number,
                TiValidators.integer,
                TiValidators.rangeValue(1, 65535),
                CustomValidators.isEqualHTTP('webHttpControl')
            ]
        ],
        webTimeoutControl: [
            { value: null, disabled: false },
            [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(5, 480)]
        ],
        selectedRadioModel: [{ value: null, disabled: false }, []],
    });

    public radioModelList: { key: string; id: string; disable: boolean }[] = [
        {
            key: this.translate.instant('SERVICE_SHARED'),
            id: 'Shared',
            disable: false
        }, {
            key: this.translate.instant('SERVICE_EXCLUSIVE'),
            id: 'Private',
            disable: false
        }
    ];

    // 自定义弹框
    public formList: { key: string; id: string; disable: boolean }[] = [
        {
            key: this.translate.instant('SERVICE_WBS_DLOG_RADIO_CSR'),
            id: 'generate',
            disable: false
        }, {
            key: this.translate.instant('SERVICE_WBS_DLOG_RADIO_INPUT_CRE'),
            id: 'getCred',
            disable: false
        }
    ];
    public selectedForm: string = 'generate';
    public generateTaskRunning: boolean = false;
    public certificateSigningRequest: boolean = false;

    public csrForm = this.fb.group({
        countryControl: [null, [TiValidators.required, Validators.pattern(/^[a-zA-Z]{2}$/)]],
        provenceControl: [null, [Validators.pattern(/^[a-zA-Z\d\-\.\_\s]{0,128}$/)]],
        cityControl: [null, [Validators.pattern(/^[a-zA-Z\d\-\.\_\s]{0,128}$/)]],
        componyControl: [null, [Validators.pattern(/^[a-zA-Z\d\-\.\_\s]{0,64}$/)]],
        departmentControl: [null, [Validators.pattern(/^[a-zA-Z\d\-\.\_\s]{0,64}$/)]],
        nameControl: [null, [TiValidators.required, Validators.pattern(/^[ a-zA-Z\d\-\.\_\s]{1,64}$/)]]
    });

    public modalOptsFooter: object = [
        {
            label: this.translate.instant('SERVICE_WBS_DLOG_GENERATE'),
            disabled: false
        },
        {
            label: this.translate.instant('ALARM_CANCEL'),
            disabled: false
        }
    ];
    public isCSR: boolean = true;
    public isServerFile: boolean = false;
    public credentialPWD: boolean = false;

    // 保存
    public webServiceDisabled: boolean = true;

    // 初始化属性
    public middleCredentialData = [];
    public rootCredentialDataShow: boolean = false;
    public webParams: object = {};
    public params: object = {};
    public HTTP: object = {};
    public HTTPS: object = {};
    public webSession: object = {};

    // SSL证书
    public serverInfo: object = [{
        id: 'serverSigner',
        label: this.translate.instant('SERVICE_WBS_CRE_SIGNER'),
        value: null
    }, {
        id: 'serverUser',
        label: this.translate.instant('SERVICE_WBS_CRE_USER'),
        value: null
    }, {
        id: 'serverDate',
        label: this.translate.instant('SERVICE_WBS_CRE_VALID_DATE'),
        value: null
    }, {
        id: 'serverSn',
        label: this.translate.instant('COMMON_SERIALNUMBER'),
        value: null
    }];

    // 根证书
    public rootCredentialData: object = [{
        id: 'rootSigner',
        label: this.translate.instant('SERVICE_WBS_CRE_SIGNER'),
        value: null
    }, {
        id: 'rootUser',
        label: this.translate.instant('SERVICE_WBS_CRE_USER'),
        value: null
    }, {
        id: 'rootDate',
        label: this.translate.instant('SERVICE_WBS_CRE_VALID_DATE'),
        value: null
    }, {
        id: 'rootSn',
        label: this.translate.instant('COMMON_SERIALNUMBER'),
        value: null
    }];

    // 导入服务器证书上传
    public inputFieldWidth = '360px';
    public certificatePassItem = {
        value: ''
    };
    public uploaderInstance;
    public uploaderConfig = {
        url: '/UI/Rest/Services/WEBService/ImportCertificate',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.getToken()
        },
        type: 'POST',
        isAutoUpload: false,
        alias: 'Content',
        filters: [{
            name: 'maxCount',
            params: [1]
        }, {
            name: 'type',
            params: ['.crt,.cer,.pem,.pfx,.p12']
        }, {
            fn: (fileItem) => {
                this.credentialPWD = false;
                if ((fileItem.type === 'pfx' || fileItem.type === 'p12') && fileItem.size <= 102400) {
                    this.credentialPWD = true;
                    return true;
                } else if (
                    (fileItem.type === 'crt' || fileItem.type === 'cer' || fileItem.type === 'pem')
                    && fileItem.size <= 1048576
                ) {
                    return true;
                } else {
                    return false;
                }
            }
        }],
        onBeforeSendItems: (fileItems) => {
            // 上传前动态添加formData
            const fileName = fileItems[0]._file.name;
            fileItems.forEach((item: TiFileItem) => {
                if (!this.credentialPWD) {
                    // 服务器证书导入，不需要密码
                    item.formData = {
                        CertificateType: 'Server',
                    };
                } else {
                    // 自定义证书导入，需要密码
                    item.formData = {
                        CertificateType: 'Custom',
                        Password: this.certificatePassItem.value
                    };
                }
            });
        },
        onRemoveItems: ($event): void => {
            this.modalOptsFooter[0].disabled = true;
        },
        onAddItemFailed: (fileItem) => {
            this.modalOptsFooter[0].disabled = false;
            this.dialogBox.close();
            if ((fileItem.file.type === 'pfx' || fileItem.file.type === 'p12') && fileItem.file.size > 102400) {
                this.alert.eventEmit.emit({ type: 'error', label: 'SERVICE_WBS_FILE_SIZE_VALI_TIP01' });
            } else if (
                (fileItem.file.type === 'crt' || fileItem.file.type === 'cer' || fileItem.file.type === 'pem')
                && fileItem.file.size > 1048576
            ) {
                this.alert.eventEmit.emit({ type: 'error', label: 'SERVICE_WBS_FILE_SIZE_VALI_TIP02' });
            } else {
                this.alert.eventEmit.emit({ type: 'error', label: 'SERVICE_UPLOADIN_CORRECT' });
            }
            if (this.uploaderInstance) {
                this.uploaderInstance.remove();
            }
        },
        onAddItemSuccess: (fileItem) => {
            this.uploaderInstance = fileItem;
            this.modalOptsFooter[0].disabled = false;
        },
        onSuccessItems: (fileItem: TiFileItem) => {
            this.modalOptsFooter[0].disabled = true;
            this.modalOptsFooter[1].disabled = true;
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.dialogBox.close();
            this.webGetInit();
        },
        onErrorItems: (event: { fileItems: TiFileItem; response: string; status: number }) => {
            const res = event.response;
            let errorMessage = 'COMMON_FAILED';
            let errorId: string;
            if (getMessageId(res).length > 0) {
                errorId = getMessageId(res)[0]?.errorId;
                errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                if (errorId === 'NoValidSession') {
                    this.timeoutService.error401.next(errorId);
                }
            }
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            this.dialogBox.close();
        }
    };
    // http端口校验
    public validationHttp: TiValidationConfig = {
        type: 'blur',
        tipPosition: 'right',
        errorMessage: {
            required: this.translate.instant('SERVICE_PORT_RANGES'),
            number: this.translate.instant('SERVICE_PORT_RANGES'),
            integer: this.translate.instant('SERVICE_PORT_RANGES'),
            rangeValue: this.translate.instant('SERVICE_PORT_RANGES'),
            isEqualTo: this.translate.instant('SERVICE_WBS_HTTP_PORT_SAME_WARN_TIP')
        }
    };
    // https端口校验
    public validationHttps: TiValidationConfig = {
        type: 'blur',
        tipPosition: 'right',
        errorMessage: {
            required: this.translate.instant('SERVICE_PORT_RANGES'),
            number: this.translate.instant('SERVICE_PORT_RANGES'),
            integer: this.translate.instant('SERVICE_PORT_RANGES'),
            rangeValue: this.translate.instant('SERVICE_PORT_RANGES'),
            isEqualTo: this.translate.instant('SERVICE_WBS_HTTPS_PORT_SAME_WARN_TIP')
        }
    };
    // 超时时长校验
    public validationTimeout: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_OVER_TIME_VALI_TIP'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('SERVICE_OVER_TIME_VALI_TIP'),
            number: this.translate.instant('SERVICE_OVER_TIME_VALI_TIP'),
            integer: this.translate.instant('SERVICE_OVER_TIME_VALI_TIP'),
            rangeValue: this.translate.instant('SERVICE_OVER_TIME_VALI_TIP'),
        }
    };

    // 国家
    public validationCountry: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_COUNTRY_FORMAT'),
        type: 'blur',
        tipPosition: 'top-left',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };
    // 省份
    public validationProvence: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_PROVINCE_FORMAT'),
        type: 'blur',
        tipPosition: 'top-left',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
        }
    };
    // 城市
    public validationCity: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_CITY_FORMAT'),
        type: 'blur',
        tipPosition: 'top-left',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
        }
    };
    // 公司
    public validationCompony: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_COMPANY_FORMAT'),
        type: 'blur',
        tipPosition: 'top-left',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
        }
    };
    // 部门
    public validationDepartment: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_DEPARTMENT_FORMAT'),
        type: 'blur',
        tipPosition: 'top-left',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
        }
    };
    // 常用名
    public validationName: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_NAME_FORMAT'),
        type: 'blur',
        tipPosition: 'top-left',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
        }
    };
    // 初始化数据查询
    public initData = (webData) => {
        // HTTP
        this.webParams['enabbleHTTPState'] = webData.HTTP.Enabled;
        // 端口
        this.webParams['portHTTPValue'] = webData.HTTP.Port;
        // HTTPS
        this.webParams['enabbleHTTPSState'] = webData.HTTPS.Enabled;
        // 端口
        this.webParams['portHTTPSValue'] = webData.HTTPS.Port;
        // 超时时长
        this.webParams['webTimeoutValue'] = webData.WebSession.TimeoutMinutes;
        // 会话模式
        this.webParams['sessionSelectedRadio'] = webData.WebSession.Mode;
        this.webServiceForm.patchValue({
            httpEnableState: this.webParams['enabbleHTTPState'],
            webHttpControl: this.webParams['portHTTPValue'],
            httpsEnableState: this.webParams['enabbleHTTPSState'],
            webHttpsControl: this.webParams['portHTTPSValue'],
            webTimeoutControl: this.webParams['webTimeoutValue'],
            selectedRadioModel: this.webParams['sessionSelectedRadio']
        });
    }

    public webGetInit = () => {
        // 获取最新预置证书标志
        this.commonService.getGenericInfo().subscribe((res) => {
            this.user.customeCertFlag = res.body.CustomeCertFlag !== undefined ? res.body.CustomeCertFlag : true;
            this.isPresetCert = !this.user.customeCertFlag;
        });
        this.httpWebService.getWeb().subscribe((response) => {
            this.loadingService.state.next(false);
            this.webData = response['body'];
            this.generateData = this.webData;
            this.initData(this.webData);
            this.drawCert(this.webData);
        }, () => {
            this.loadingService.state.next(false);
        });
    }
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
            this.webAuthority();
        }
    }
    // 权限
    public webAuthority() {
        // 权限控制具有安全配置权限可以设置HTTP及端口、HTTPS及端口、服务器证书信息
        if (this.isOemSecurityMgmtUser && !this.isSystemLock) {
            this.webServiceForm.root.get('httpEnableState').enable();
            this.webServiceForm.root.get('webHttpControl').enable();
            this.webServiceForm.root.get('httpsEnableState').enable();
            this.webServiceForm.root.get('webHttpsControl').enable();
        } else {
            this.webServiceForm.root.get('httpEnableState').disable();
            this.webServiceForm.root.get('webHttpControl').disable();
            this.webServiceForm.root.get('httpsEnableState').disable();
            this.webServiceForm.root.get('webHttpsControl').disable();
            if (this.dialogBox) {
                this.dialogBox.dismiss();
            }
        }

        // 权限控制具有常规设置权限可以设置超时时长、会话模式
        if (this.isConfigureComponentsUser && !this.isSystemLock) {
            this.webServiceForm.root.get('webTimeoutControl').enable();
            this.webServiceForm.root.get('selectedRadioModel').enable();
        } else {
            this.webServiceForm.root.get('webTimeoutControl').disable();
            this.webServiceForm.root.get('selectedRadioModel').disable();
        }
    }
    // 初始化
    ngOnInit(): void {
        this.loadingService.state.next(true);
        this.webAuthority();

        this.webGetInit();
    }

    // 证书信息查询
    public drawCert(webData): void {
        // 证书信息
        this.serverInfo[0].value = webData.ServerCert.Issuer;
        this.serverInfo[1].value = webData.ServerCert.Subject;
        this.serverInfo[2].value =
            webData.ServerCert.ValidNotBefore + this.translate.instant('SERVICE_WBS_CRE_VALID_DATE_TO') + webData.ServerCert.ValidNotAfter;
        this.serverInfo[3].value = webData.ServerCert.SerialNumber;
        const RootCert = webData.RootCert;
        this.certificateSigningRequest = webData.ServerCert.CertificateSigningRequest ? true : false;

        // 根证书
        if (RootCert) {
            this.rootCredentialDataShow = true;
            this.rootCredentialData[0].value = RootCert.Issuer;
            this.rootCredentialData[1].value = RootCert.Subject;
            this.rootCredentialData[2].value =
                RootCert.ValidNotBefore + this.translate.instant('SERVICE_WBS_CRE_VALID_DATE_TO') + RootCert.ValidNotAfter;
            this.rootCredentialData[3].value = RootCert.SerialNumber;
        } else {
            this.rootCredentialDataShow = false;
        }
        // 中间证书
        const intermediateCert = webData.IntermediateCert;
        this.middleCredentialData = [];
        if (intermediateCert && JSON.stringify(intermediateCert) !== '{}') {
            for (const key of intermediateCert) {
                const item = {
                    issuser: [this.translate.instant('SERVICE_WBS_CRE_SIGNER'), key.Issuer],
                    subject: [this.translate.instant('SERVICE_WBS_CRE_USER'), key.Subject],
                    startEnd: [
                        this.translate.instant('SERVICE_WBS_CRE_VALID_DATE'),
                        key.ValidNotBefore + this.translate.instant('SERVICE_WBS_CRE_VALID_DATE_TO') + key.ValidNotAfter
                    ],
                    serial: [this.translate.instant('COMMON_SERIALNUMBER'), key.SerialNumber]
                };
                this.middleCredentialData.push(item);
            }
        }
    }

    // 数据是否发生变化
    public protocolEnabledEN = () => {
        if (!this.webServiceForm.valid) {
            this.webServiceDisabled = true;
            return;
        }
        this.params = {};
        this.HTTP = {};
        this.HTTPS = {};
        this.webSession = {};
        if (this.webServiceForm.root.get('httpEnableState').value !== this.webParams['enabbleHTTPState']) {
            this.HTTP['Enabled'] = this.webServiceForm.root.get('httpEnableState').value;
        }
        if (parseInt(this.webServiceForm.root.get('webHttpControl').value, 10) !== this.webParams['portHTTPValue']) {
            this.HTTP['Port'] = parseInt(this.webServiceForm.root.get('webHttpControl').value, 10);
        }
        if (this.webServiceForm.root.get('httpsEnableState').value !== this.webParams['enabbleHTTPSState']) {
            this.HTTPS['Enabled'] = this.webServiceForm.root.get('httpsEnableState').value;
        }
        if (parseInt(this.webServiceForm.root.get('webHttpsControl').value, 10) !== this.webParams['portHTTPSValue']) {
            this.HTTPS['Port'] = parseInt(this.webServiceForm.root.get('webHttpsControl').value, 10);
        }
        if (parseInt(this.webServiceForm.root.get('webTimeoutControl').value, 10) !== this.webParams['webTimeoutValue']) {
            this.webSession['TimeoutMinutes'] = parseInt(this.webServiceForm.root.get('webTimeoutControl').value, 10);
        }
        if (this.webServiceForm.root.get('selectedRadioModel').value !== this.webParams['sessionSelectedRadio']) {
            this.webSession['Mode'] = this.webServiceForm.root.get('selectedRadioModel').value;
        }
        if (this.isnotEmptyObject(this.HTTP)) {
            this.params['HTTP'] = this.HTTP;
        }
        if (this.isnotEmptyObject(this.HTTPS)) {
            this.params['HTTPS'] = this.HTTPS;
        }
        if (this.isnotEmptyObject(this.webSession)) {
            this.params['WebSession'] = this.webSession;
        }
        if (JSON.stringify(this.params) !== '{}') {
            this.webServiceDisabled = false;
        } else {
            this.webServiceDisabled = true;
        }
    }

    // 判断对象是否为空
    public isnotEmptyObject(obj): boolean {
        let res = false;
        for (const k in obj) {
            if (Object.prototype.hasOwnProperty.call(obj, k)) {
                res = true;
            }
        }
        return res;
    }

    // change事件
    public httpEnableIdChange = () => {
        this.protocolEnabledEN();
    }
    public httpPortChange = () => {
        this.webServiceForm.get('webHttpsControl').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    public httpsEnableIdChange = () => {
        this.protocolEnabledEN();
    }
    public httpsPortChange = () => {
        this.webServiceForm.get('webHttpControl').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    public webTimeoutChange = () => {
        this.protocolEnabledEN();
    }
    public sessionModelChange = () => {
        this.protocolEnabledEN();
    }

    // 展开收起
    public clickFn = () => {
        this.isCollapsed = !this.isCollapsed;
        this.dropDown = !this.dropDown;
    }

    // 恢复默认值
    public resetHttpPort(): void {
        this.webServiceForm.patchValue({
            webHttpControl: 80,
        });
        this.protocolEnabledEN();
    }
    public resetHttpsPort(): void {
        this.webServiceForm.patchValue({
            webHttpsControl: 443,
        });
        this.protocolEnabledEN();
    }
    // 保存点击事件
    public webServiceChangeDisabled(): void {
        this.webServiceDisabled = true;
        this.protocolEnabledEN();
        // 在HTTPS有更改且关闭下才提示
        if (this.params['HTTPS'] && this.HTTPS['Enabled'] === false) {
            this.tiMessage.open({
                id: 'httpsStateMessage',
                type: 'prompt',
                title: this.translate.instant('COMMON_CONFIRM'),
                content: this.translate.instant('SERVICE_WBS_HTTPS_TIP'),
                close: () => {
                    this.saveWebInfo();
                },
                dismiss: () => { }
            });
        } else {
            this.saveWebInfo();
        }
    }
    public saveWebInfo() {
        this.httpWebService.setWebService(this.params).subscribe((response) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.webData = response['body'];
            this.initData(this.webData);
            if (this.params && this.params['HTTPS'] && this.params['HTTPS']['Port']) {
                window.location.port = response['body'].HTTPS.Port;
            }
        }, (error) => {
            const errorId = getMessageId(error.error)[0].errorId;
            const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            this.webData = error.error.data;
            this.initData(this.webData);
        });
    }

    // 自定义证书 生成CSR文件
    public userDefined(content): void {
        this.credentialPWD = false;
        if (this.generateData && this.generateData['ServerCert']) {
            const subject = this.generateData['ServerCert'].Subject;
            const subjects = subject.split(',');
            for (const key of subjects) {
                const subjectArr = key.split('=');
                if (subjectArr[0].trim() === 'C') {
                    this.csrForm.patchValue({ countryControl: subjectArr[1] });
                } else if (subjectArr[0].trim() === 'S') {
                    this.csrForm.patchValue({ provenceControl: subjectArr[1] });
                } else if (subjectArr[0].trim() === 'L') {
                    this.csrForm.patchValue({ cityControl: subjectArr[1] });
                } else if (subjectArr[0].trim() === 'O') {
                    this.csrForm.patchValue({ componyControl: subjectArr[1] });
                } else if (subjectArr[0].trim() === 'OU') {
                    this.csrForm.patchValue({ departmentControl: subjectArr[1] });
                } else if (subjectArr[0].trim() === 'CN') {
                    this.csrForm.patchValue({ nameControl: subjectArr[1] });
                }
            }
        }
        TiValidators.check(this.csrForm);
        this.modalOptsFooter[0].disabled = !this.csrForm.valid;
        this.dialogBox = this.tiModal.open(content, {
            id: 'fileCRSId',
            context: {
                close: (): void => {
                    if (this.selectedForm === 'generate') {
                        this.modalOptsFooter[0].disabled = true;
                        const param = {
                            Country: this.csrForm.root.get('countryControl').value,
                            CommonName: this.csrForm.root.get('nameControl').value,
                            City: this.csrForm.root.get('cityControl').value,
                            State: this.csrForm.root.get('provenceControl').value,
                            OrgName: this.csrForm.root.get('componyControl').value,
                            OrgUnit: this.csrForm.root.get('departmentControl').value
                        };

                        // 如果已经存在了证书
                        if (this.certificateSigningRequest) {
                            const instance = this.tiMessage.open({
                                id: 'sslCert',
                                type: 'prompt',
                                content: this.translate.instant('SERVICE_SSL_CERT_EXIST'),
                                okButton: {
                                    show: true,
                                    text: this.translate.instant('ALARM_OK'),
                                    autofocus: false,
                                    click: () => {
                                        instance.close();
                                        this.recursionCsr(param);
                                    }
                                },
                                cancelButton: {
                                    show: true,
                                    text: this.translate.instant('ALARM_CANCEL'),
                                    click(): void {
                                        instance.dismiss();
                                    }
                                },
                                title: this.translate.instant('ALARM_OK'),
                            });
                        } else {
                            this.recursionCsr(param);
                        }
                    } else {
                        this.uploaderInstance.upload();
                    }
                    this.certificatePassItem.value = '';
                },
                dismiss: (): void => {
                    this.dialogBox.dismiss();
                    this.certificatePassItem.value = '';
                }
            }
        });
    }
    public taskCsr(taskUrl, params): void {
        this.httpWebService.task(taskUrl).subscribe((taskData) => {
            if (taskData['body'].prepare_progress === 100 && taskData['body'].downloadurl) {
                const downloadurl = taskData['body'].downloadurl;
                // 下载
                this.commonService.restDownloadFile(downloadurl);
                this.generateTaskRunning = false;
                this.modalOptsFooter[0].disabled = false;
                this.dialogBox.close();
            } else if (taskData['body'].prepare_progress <= 100) {
                setTimeout(() => {
                    this.taskCsr(taskUrl, params);
                }, 3000);
            }
        });
    }
    public recursionCsr(params): void {
        this.httpWebService.exportCSR(params).subscribe((res) => {
            this.generateTaskRunning = true;
            const taskUrl = res['body'].url;
            this.taskCsr(taskUrl, params);
        }, (error) => {
            this.modalOptsFooter[0].disabled = false;
            this.generateTaskRunning = false;
            this.dialogBox.close();
        });
    }
    public generateDetailData(): void {
        if (!this.csrForm.valid) {
            this.modalOptsFooter[0].disabled = true;
        } else {
            this.modalOptsFooter[0].disabled = false;
        }
    }
    // 点击按钮改变对应的内容
    public changeGetcred(val): void {
        if (val === 'generate') {
            this.isCSR = true;
            this.isServerFile = false;
            this.modalOptsFooter[0].label = this.translate.instant('SERVICE_WBS_DLOG_GENERATE');
            TiValidators.check(this.csrForm);
            this.modalOptsFooter[0].disabled = !this.csrForm.valid;
        }
        if (val === 'getCred') {
            this.isCSR = false;
            this.isServerFile = true;
            this.modalOptsFooter[0].label = this.translate.instant('COMMON_OK');
            this.bfRadioChangeState = this.modalOptsFooter[0].disabled;
            this.modalOptsFooter[0].disabled = true;
        }
    }
}
