import { Component, OnInit, TemplateRef } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { CertUpdateService } from '../../services';
import { Validators, UntypedFormBuilder } from '@angular/forms';
import { TiValidators, TiFileItem, TiFileInfo, TiValidationConfig, TiModalService } from '@cloud/tiny3';
import {
    AlertMessageService,
    LoadingService,
    UserInfoService,
    ErrortipService,
    TimeoutService
} from 'src/app/common-app/service';
import { MultVaild } from 'src/app/common-app/utils/multValid';
import { getMessageId, getBrowserType, DESCRIPTION, SerAddrValidators } from 'src/app/common-app/utils';

@Component({
    selector: 'app-cert-update-ssl',
    templateUrl: './cert-update-ssl.component.html',
    styleUrls: ['./cert-update-ssl.component.scss']
})
export class CertUpdateSslComponent implements OnInit {
    constructor(
        private translate: TranslateService,
        private certUpdateService: CertUpdateService,
        private tiModal: TiModalService,
        private alert: AlertMessageService,
        private fb: UntypedFormBuilder,
        private user: UserInfoService,
        private errorTip: ErrortipService,
        private timeoutService: TimeoutService,
        private loadingService: LoadingService
    ) {}
    public browser: string = getBrowserType().browser;
    public dialogInstance = null;
    public file: any;

    // 初始化属性
    public showAlert = {
        type: 'warn',
        openAlert: false,
        label: this.translate.instant('SERVICE_VNC_SECURITY_TIP')
    };

    // 安全配置权限
    public isOemSecurityMgmtUser = this.user.hasPrivileges(['OemSecurityMgmt']);

    // 系统是否锁定
    public isSystemLock = this.user.systemLockDownEnabled;

    public uploadText: string = 'COMMON_UPLOAD';

    // 保存状态
    public certUpdateData: object = {};

    // 自动更新使能和证书更新按钮是否可用，CA服务器配置好了,且有安全配置权限，以及没有系统锁定
    public caCanUpdate: boolean = false;

    // 服务器地址
    public ipPoliceValidation: TiValidationConfig = {
        tip: this.translate.instant('COMMON_IPV4_IPV6_DOMIN_CERT_UPDATE'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            maxLength: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };

    // 端口
    public portValidation: TiValidationConfig = {
        tip: this.translate.instant('VALIDTOR_RANGE'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('SERVICE_PORT_RANGES'),
            number: this.translate.instant('SERVICE_PORT_RANGES'),
            integer: this.translate.instant('SERVICE_PORT_RANGES'),
            rangeValue: this.translate.instant('SERVICE_PORT_RANGES')
        }
    };

    // 路径
    public pathValidation: TiValidationConfig = {
        tip: this.translate.instant('IBMC_CA_SERVER_CMP_PATH_TIP'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };

    // 国家
    public countryValidation: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_COUNTRY_FORMAT'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };

    // 省份
    public provinceValidation: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_PROVINCE_FORMAT'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };

    // 城市
    public cityValidation: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_CITY_FORMAT'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };

    // 公司
    public companyValidation: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_COMPANY_FORMAT'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };

    // 部门
    public departmentValidation: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_DEPARTMENT_FORMAT'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };

    // 常用名
    public commonNameValidation: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_NAME_FORMAT'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };

    // 内部名
    public internalNameValidation: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_NAME_INNER'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };

    // 邮件地址
    public emailAddressValidation: TiValidationConfig = {
        tip: this.translate.instant('ALARM_REPORT_EMAIL_TIP'),
        type: 'blur',
        errorMessage: {
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            emailValidator: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };

    // 上传按钮状态
    public uploadBtnStatus: boolean = true;
    public fileName: any;
    public errorTips: string = '';
    public isAddFileError: boolean = false;

    // 客户端证书 上传
    public cer = {
        label1: 'ALARM_REPORT_SYSLOG_CER_FILE',
        label2: 'SERVICE_WBS_DLOG_CRE_PWD',
        tip: 'ALARM_REPORT_SYSLOG_CER_TIP',
        value: '',
        url: '',
        filters: [
            {
                name: 'maxCount',
                params: [1]
            },
            {
                name: 'type',
                params: ['.pfx,.p12']
            },
            {
                name: 'maxSize',
                params: [100 * 1024]
            }
        ],
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
            if (validResults && validResults instanceof Array) {
                const validResultValue = validResults[0];
                this.isAddFileError = true;
                this.uploadBtnStatus = true;
                if (validResultValue === 'maxSize') {
                    this.errorTips = 'COMMON_UPLOAD_FILE_MORN_1M_ERROR4';
                } else if (validResultValue === 'type') {
                    this.errorTips = 'IBMC_UPLOAD_FILE_TYPE';
                }
            }
        },
        save: (context) => {
            if (this.file) {
                this.updateServerCertificate(this.file, context);
            }
        }
    };

    // 根证书上传
    public rootUpload = {
        url: '/UI/Rest/AccessMgnt/CertUpdateService/ImportCA',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.getToken()
        },
        type: 'POST',
        filters: [
            {
                name: 'type',
                params: ['.crt,.cer,.pem']
            },
            {
                name: 'maxSize',
                params: [100 * 1024]
            }
        ],
        autoUpload: true,
        alias: 'Content',
        onAddItemSuccess: (data: TiFileItem) => {
            this.loadingService.state.next(true);
        },
        successItems: (data: TiFileItem) => {
            this.alert.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS'
            });
            this.saveLog();
        },
        addItemFailed: (data: TiFileInfo) => {
            this.loadingService.state.next(false);
            const validResults = data['validResults'];
            if (validResults && validResults instanceof Array) {
                const validResultValue = validResults[0];
                if (validResultValue === 'maxSize') {
                    this.alert.eventEmit.emit({
                        type: 'error',
                        label: 'COMMON_UPLOAD_FILE_MORN_1M_ERROR4'
                    });
                } else if (validResultValue === 'type') {
                    this.alert.eventEmit.emit({
                        type: 'error',
                        label: 'COMMON_FILE_SUFFIX_CER_PEM_CRT'
                    });
                }
            }
        },
        errorItems: (fileItem) => {
            this.loadingService.state.next(false);
            const res = fileItem.response;
            const messageIdRes = getMessageId(res);
            if (messageIdRes && messageIdRes instanceof Array && messageIdRes.length) {
                const errorId = messageIdRes[0].errorId;
                const errorMessage = errorId
                    ? this.errorTip.getErrorMessage(errorId) || 'COMMON_FAILED'
                    : 'COMMON_FAILED';
                if (errorId === 'NoValidSession') {
                    this.timeoutService.error401.next(errorId);
                }
                this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            } else {
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            }
        }
    };

    // 证书吊销列表 上传按钮的配置，更多的配置在certificate.component.ts文件中的cerInvalid对象中
    public cerInvalidConfig = {
        // 上传 的地址
        url: '/UI/Rest/AccessMgnt/CertUpdateService/ImportCrl'
    };

    // 根证书 展示信息
    public oneWayInfo = {
        title: 'SERVICE_WBS_SERVER_CRE_INFO_CERT_UPDATE',
        issuer: '',
        subject: '',
        validNotBefore: '',
        validNotAfter: '',
        serialNumber: '',
        crlState: false,
        listTime: '',
        crlURL: '',
        deleteCrlUrl: '/UI/Rest/AccessMgnt/CertUpdateService/DeleteCrl'
    };

    // 客户端证书 展示信息
    public twoWayInfo = {
        title: 'SERVICE_WBS_SERVER_CRE_INFO_CERT_UPDATE',
        issuer: '',
        subject: '',
        validNotBefore: '',
        validNotAfter: '',
        serialNumber: ''
    };

    public params = {
        SubjectInfo: {}
    };
    // 保存按钮是否可以点击
    public canSave = false;
    // 测试按钮是否展示
    public showTestBtn = true;
    // 是否完成数据初始化
    public hadInit = false;

    public preData = {
        AutoUpdateEnabled: false,
        CAServerAddr: '',
        CAServerPort: '',
        SubjectInfo: {
            CommonName: '',
            Country: '',
            // 省份
            State: '',
            // 公司
            OrgName: '',
            // 部门
            OrgUnit: '',
            // 城市
            Location: '',
            InternalName: '',
            Email: ''
        }
    };

    public formGroup = this.fb.group({
        autoUpdateEnable: [false],
        ipPoliceControl: [
            '',
            [
                TiValidators.required,
                TiValidators.maxLength(67),
                SerAddrValidators.validateIp46AndDomin(),
                SerAddrValidators.validateIp4Exclude(['255.255.255.255'])
            ]
        ],
        portControl: [
            '',
            [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 65535)]
        ],
        pathControl: ['', [TiValidators.required, Validators.pattern(/^\/[A-Za-z0-9_/]{1,127}$/)]],
        countryControl: ['', [TiValidators.required, Validators.pattern(/^[a-zA-Z]{2}$/)]],
        provinceControl: ['', [Validators.pattern(/^[a-zA-Z\d\-\.\_\s]{0,128}$/)]],
        cityControl: ['', [Validators.pattern(/^[a-zA-Z\d\-\.\_\s]{0,128}$/)]],
        companyControl: ['', [Validators.pattern(/^[a-zA-Z\d\-\.\_\s]{0,64}$/)]],
        departmentControl: ['', [Validators.pattern(/^[a-zA-Z\d\-\.\_\s]{0,64}$/)]],
        commonNameControl: ['', [TiValidators.required, Validators.pattern(/^[ a-zA-Z\d\-\.\_\s]{1,64}$/)]],
        internalNameControl: ['', [Validators.pattern(/^[ a-zA-Z\d\-\.\_\s]{0,64}$/)]],
        emailAddressControl: ['', [MultVaild.emailValidator(), Validators.pattern(DESCRIPTION)]]
    });

    public crlUpload() {}

    // 吊销证书上传 或者 客户端证书上传完成后，重新获取数据
    public saveLog() {
        this.getData();
    }

    // 数据是否发生变化
    public protocolEnabledEN() {
        // 没有完成数据初始化前，不让触发对保存按钮的状态计算
        if (!this.hadInit) {
            return;
        }
        // 必填
        const ipPolice = this.formGroup.root.get('ipPoliceControl').value;
        const port = this.formGroup.root.get('portControl').value;
        const path = this.formGroup.root.get('pathControl').value;
        const country = this.formGroup.root.get('countryControl').value;
        const commonName = this.formGroup.root.get('commonNameControl').value;

        // 非必填
        const province = this.formGroup.root.get('provinceControl').value;
        const city = this.formGroup.root.get('cityControl').value;
        const company = this.formGroup.root.get('companyControl').value;
        const department = this.formGroup.root.get('departmentControl').value;
        const internalName = this.formGroup.root.get('internalNameControl').value;
        const emailAddress = this.formGroup.root.get('emailAddressControl').value;

        this.params = {
            SubjectInfo: {}
        };

        // 服务器地址
        if (ipPolice !== this.preData['CAServerAddr']) {
            this.params['CAServerAddr'] = ipPolice;
        }

        // 端口
        let portNum: any = '';
        if (port || port === 0) {
            portNum = parseInt(port, 10);
        }
        if (portNum !== this.preData['CAServerPort']) {
            this.params['CAServerPort'] = portNum;
        }

        // 路径
        if (path !== this.preData['CAServerCMPPath']) {
            this.params['CAServerCMPPath'] = path;
        }

        // 标记证书主题是否发生修改
        let flag = false;

        // 国家(C)
        if (country !== this.preData.SubjectInfo.Country) {
            this.params.SubjectInfo['Country'] = country;
            flag = true;
        } else {
            this.params.SubjectInfo['Country'] = this.preData.SubjectInfo.Country;
        }

        // 省份(S)
        if (province !== this.preData.SubjectInfo.State) {
            this.params.SubjectInfo['State'] = province;
            flag = true;
        }

        // 	城市(L)
        if (city !== this.preData.SubjectInfo.Location) {
            this.params.SubjectInfo['Location'] = city;
            flag = true;
        }

        // 	公司(O)
        if (company !== this.preData.SubjectInfo.OrgName) {
            this.params.SubjectInfo['OrgName'] = company;
            flag = true;
        }

        // 部门(OU)
        if (department !== this.preData.SubjectInfo.OrgUnit) {
            this.params.SubjectInfo['OrgUnit'] = department;
            flag = true;
        }

        // 常用名(CN)
        if (commonName !== this.preData.SubjectInfo.CommonName) {
            this.params.SubjectInfo['CommonName'] = commonName;
            flag = true;
        } else {
            this.params.SubjectInfo['CommonName'] = this.preData.SubjectInfo.CommonName;
        }

        // 内部名(IN)
        if (internalName !== this.preData.SubjectInfo.InternalName) {
            this.params.SubjectInfo['InternalName'] = internalName;
            flag = true;
        }

        // 邮件地址
        if (emailAddress !== this.preData.SubjectInfo.Email) {
            this.params.SubjectInfo['Email'] = emailAddress;
            flag = true;
        }

        // 证书主题没有发生修改则不传SubjectInfo对象
        if (!flag) {
            delete this.params.SubjectInfo;
        }

        // 如果数据没有发生修改
        if (JSON.stringify(this.params) === '{}') {
            this.canSave = false;
            this.showTestBtn = true;
        } else if (!this.formGroup.valid) {
            // 如果数据发生修改且存在数据校验不通过的
            this.canSave = false;
            this.showTestBtn = false;
        } else {
            this.canSave = true;
            this.showTestBtn = false;
        }
    }

    // 自动更新使能修改
    public autoUpdateEnableChange() {
        // 没有初始化好不执行后续代码
        if (!this.hadInit) {
            return;
        }
        const state = this.formGroup.root.get('autoUpdateEnable').value;
        if (this.preData.AutoUpdateEnabled === state) {
            return;
        }

        const params = {
            AutoUpdateEnabled: state
        };

        this.hadInit = false;
        this.loadingService.state.next(true);
        this.certUpdateService.certUpdateSaveData({ CMPConfig: params }).subscribe(
            (response) => {
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                this.certUpdateData = response['body'];
                this.initData(this.certUpdateData);
            },
            () => {
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                this.loadingService.state.next(false);
                // 失败的时候取消开关的修改
                this.formGroup.patchValue({
                    autoUpdateEnable: !state
                });
                this.hadInit = true;
            },
            () => {
                this.loadingService.state.next(false);
            }
        );
    }

    // 点击证书更新
    public updateCertFromCA() {
        this.loadingService.state.next(true);
        this.certUpdateService.updateCertFromCA().subscribe(
            (response) => {
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            },
            (err) => {
                if (err.error) {
                    const errorId = getMessageId(err.error)?.[0]?.errorId;
                    if (errorId) {
                        const errorMessage = this.errorTip.getErrorMessage(errorId) || 'COMMON_FAILED';
                        this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                        this.loadingService.state.next(false);
                        return;
                    }
                }

                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                this.loadingService.state.next(false);
            },
            () => {
                this.loadingService.state.next(false);
            }
        );
    }

    // 点击 客户端证书 的上传按钮
    public uploadDialogFun(context: TemplateRef<any>) {
        this.isAddFileError = false;
        this.uploadBtnStatus = true;
        this.cer.value = '';
        this.dialogInstance = this.tiModal.open(context, {
            // 修改下面这个id需要注意去zh-CN.scss文件里面搜索uploadDialog,根据需要把修改后的id也加上那些样式属性，对应的其他小语种的scss文件也是类似的处理
            id: 'uploadDialog'
        });
    }

    // 取消更新 客户端证书
    public btnCancel(context) {
        context.close();
    }

    // 确认更新 客户端证书
    public updateServerCertificate(file: string, context) {
        const clientFormData = new FormData();
        clientFormData.append('Content', file, this.fileName);
        if (this.cer.value) {
            clientFormData.append('Password', this.cer.value);
        }

        this.loadingService.state.next(true);
        this.certUpdateService
            .imporCustomCertificate(clientFormData, {
                headers: {
                    From: 'WebUI',
                    Accept: '*/*',
                    'X-CSRF-Token': this.user.getToken()
                }
            })
            .subscribe(
                () => {
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    context.close();
                    this.saveLog();
                },
                (err) => {
                    if (err.error) {
                        const errorId = getMessageId(err.error)?.[0]?.errorId;
                        if (errorId && errorId.indexOf('EncryptionAlgorithmIsWeak') > -1) {
                            this.alert.eventEmit.emit({ type: 'error', label: 'ALARM_REPORT_SYSLOG_MD5_CARD_FAILED' });
                        } else {
                            const errorMessage = this.errorTip.getErrorMessage(errorId) || 'COMMON_FAILED';
                            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                        }
                    } else {
                        this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                    }
                    context.close();
                    this.loadingService.state.next(false);
                }
            );
    }

    // 点击保存
    public save() {
        this.loadingService.state.next(true);
        this.certUpdateService.certUpdateSaveData({ CMPConfig: this.params }).subscribe(
            (response) => {
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                this.certUpdateData = response['body'];
                this.initData(this.certUpdateData);
            },
            (err) => {
                if (err.error) {
                    const errorId = getMessageId(err.error)?.[0]?.errorId;
                    if (errorId) {
                        const errorMessage = this.errorTip.getErrorMessage(errorId) || 'COMMON_FAILED';
                        this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                        this.loadingService.state.next(false);
                        return;
                    }
                }
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                this.loadingService.state.next(false);
            },
            () => {
                this.loadingService.state.next(false);
            }
        );
    }

    // 点击取消
    public cancel() {
        this.getData();
    }

    // 把对象中是null的属性改为空字符串
    public setEmptyStr(obj: any) {
        if (typeof obj === 'object' && obj instanceof Object) {
            for (const key of Object.keys(obj)) {
                const value = obj[key];
                if (value === null) {
                    obj[key] = '';
                }
            }
        }
    }

    // 初始化数据查询
    public initData(certUpdateData) {
        const cmpConfig = certUpdateData?.CMPConfig;
        // 基本信息
        if (cmpConfig) {
            // 主题配置数据
            const subjectInfo = cmpConfig.SubjectInfo;
            this.setEmptyStr(subjectInfo);

            this.formGroup.patchValue({
                autoUpdateEnable: cmpConfig.AutoUpdateEnabled,
                ipPoliceControl: cmpConfig.CAServerAddr,
                portControl: cmpConfig.CAServerPort,
                pathControl: cmpConfig?.CAServerCMPPath,
                countryControl: subjectInfo?.Country,
                provinceControl: subjectInfo?.State,
                cityControl: subjectInfo?.Location,
                companyControl: subjectInfo?.OrgName,
                departmentControl: subjectInfo?.OrgUnit,
                commonNameControl: subjectInfo?.CommonName,
                internalNameControl: subjectInfo?.InternalName,
                emailAddressControl: subjectInfo?.Email
            });

            this.formGroup.setErrors(null);
            this.formGroup.disable();
            this.formGroup.markAsUntouched();
            this.formGroup.markAsPristine();
            this.formGroup.enable();

            Object.assign(this.preData, cmpConfig);
        }

        let serverCertFlag = false;
        // 服务器根证书 的 证书信息
        const chainInfo = certUpdateData?.CACertChainInfo;
        if (chainInfo) {
            const serverCert = chainInfo.ServerCert;
            // 序列号存在则代表已上传了证书，反之则没有上传，不展示对应的证书信息
            const serialNumber = serverCert?.SerialNumber;
            if (serialNumber) {
                serverCertFlag = true;
                this.oneWayInfo.issuer = serverCert.IssueBy;
                this.oneWayInfo.subject = serverCert.IssueTo;
                this.oneWayInfo.validNotBefore = serverCert.ValidFrom;
                this.oneWayInfo.validNotAfter = serverCert.ValidTo;
                this.oneWayInfo.serialNumber = serialNumber;
                this.oneWayInfo.crlState = serverCert.IsImportCrl;
                if (serverCert.CrlValidFrom && serverCert.CrlValidTo) {
                    this.oneWayInfo.listTime = `${serverCert.CrlValidFrom} - ${serverCert.CrlValidTo}`;
                } else {
                    this.oneWayInfo.listTime = '--';
                }
            }
        }

        let clientCertFlag = false;
        // 客户端证书 的 证书信息
        const clientCertChainInfo = certUpdateData?.ClientCertChainInfo;
        if (clientCertChainInfo) {
            const clientCert = clientCertChainInfo.ClientCert;
            const serialNumber = clientCert?.SerialNumber;
            if (serialNumber) {
                clientCertFlag = true;
                this.twoWayInfo.issuer = clientCert.IssueBy;
                this.twoWayInfo.subject = clientCert.IssueTo;
                this.twoWayInfo.validNotBefore = clientCert.ValidFrom;
                this.twoWayInfo.validNotAfter = clientCert.ValidTo;
                this.twoWayInfo.serialNumber = serialNumber;
            }
        }

        this.canSave = false;
        this.showTestBtn = true;
        this.hadInit = true;

        // 获取到的数据要满足校验；有服务器证书和客户端证书
        this.caCanUpdate = this.formGroup.valid && serverCertFlag && clientCertFlag;

        if (this.isOemSecurityMgmtUser && !this.isSystemLock && this.caCanUpdate) {
            this.formGroup.root.get('autoUpdateEnable').enable();
        } else {
            this.formGroup.root.get('autoUpdateEnable').disable();
        }

        this.authorityAction();
    }

    // 系统锁定状态变化
    public lockStateChange(state: boolean) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
            this.authorityAction();
        }
    }

    // 根据权限和锁定状态设置页面表单元素的可用状态
    public authorityAction() {
        // 具有安全配置权限可以设置自动更新使能、服务器地址、端口、证书主题
        if (this.isOemSecurityMgmtUser && !this.isSystemLock) {
            this.formGroup.root.get('ipPoliceControl').enable();
            this.formGroup.root.get('portControl').enable();
            this.formGroup.root.get('pathControl').enable();
            this.formGroup.root.get('countryControl').enable();
            this.formGroup.root.get('provinceControl').enable();
            this.formGroup.root.get('cityControl').enable();
            this.formGroup.root.get('companyControl').enable();
            this.formGroup.root.get('departmentControl').enable();
            this.formGroup.root.get('commonNameControl').enable();
            this.formGroup.root.get('internalNameControl').enable();
            this.formGroup.root.get('emailAddressControl').enable();
        } else {
            this.formGroup.root.get('ipPoliceControl').disable();
            this.formGroup.root.get('portControl').disable();
            this.formGroup.root.get('pathControl').disable();
            this.formGroup.root.get('countryControl').disable();
            this.formGroup.root.get('provinceControl').disable();
            this.formGroup.root.get('cityControl').disable();
            this.formGroup.root.get('companyControl').disable();
            this.formGroup.root.get('departmentControl').disable();
            this.formGroup.root.get('commonNameControl').disable();
            this.formGroup.root.get('internalNameControl').disable();
            this.formGroup.root.get('emailAddressControl').disable();
        }
    }

    // 获取数据
    public getData() {
        this.loadingService.state.next(true);
        this.hadInit = false;
        this.certUpdateService.getData().subscribe(
            (response) => {
                this.certUpdateData = response['body'];
                this.initData(this.certUpdateData);
            },
            () => {
                this.loadingService.state.next(false);
            },
            () => {
                this.loadingService.state.next(false);
            }
        );
    }

    ngOnInit(): void {
        this.authorityAction();
        this.getData();
    }
}
