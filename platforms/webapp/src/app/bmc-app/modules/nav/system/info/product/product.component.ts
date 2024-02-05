import { TranslateService } from '@ngx-translate/core';
import { Component, OnInit } from '@angular/core';
import { Subscription } from 'rxjs';
import { ProductService } from './service';
import { IItem, productId, mainBoardId, systemId, DigitalWarrantyId, IInfo } from './product.datatype';
import { UntypedFormControl } from '@angular/forms';
import Validator from './validator';
import Date from 'src/app/common-app/utils/date';
import { formatEntry } from 'src/app/common-app/utils';
import { TiValidators } from '@cloud/tiny3';
import {
    AlertMessageService,
    LoadingService,
    UserInfoService,
    CommonService,
    HttpService,
    GlobalDataService,
    PRODUCTTYPE
} from 'src/app/common-app/service';

@Component({
    selector: 'app-product',
    templateUrl: './product.component.html',
    styleUrls: ['./product.component.scss']
})
export class ProductComponent implements OnInit {

    public colsNumber = 2;

    public verticalAlign = 'middle';

    public labelWidth = '';

    public colsGap = ['200px'];

    public showEditIcon = this.userServ.hasPrivileges(['ConfigureComponents']);

    public systemLockDownEnabled = this.userServ.systemLockDownEnabled;

    public showSmsEnabled = true;

    public showSysInfo = false;

    public showSysInfoState = true;

    // 是否天池板（隐藏主板信息显示、产品信息中显示“产品编码”
    public isTianchiBoard = false;

    public mianBoardTitle: string;
    private queryInfoSub: Subscription;

    public loaded = false;

    private i18n = {
        COMMON_FORMAT_ERROR: this.translate.instant('COMMON_FORMAT_ERROR'),
    };

    private validator = new Validator(this.i18n);
     public isAtlas1951: boolean = false;

    // 产品信息
    public productInfos: IItem[] = [
        {
            label: 'INFO_PRODUCT_NAME',
            id: productId.productInfoName,
            content: ''
        },
        {
            label: 'INFO_MANUFACTURER',
            id: productId.productManufacturer,
            content: ''
        },
        {
            label: 'INFO_PRODUCT_SERIAL_NUMBER',
            id: productId.productSerialNum,
            content: ''
        },
        {
            label: 'INFO_SYSTEM_NUMBER',
            id: productId.systemSN,
            content: '',
            hide: true
        },
        {
            label: 'OTHER_PART_NUMBER',
            id: productId.bomNumber,
            content: ''
        },
        {
            label: 'INFO_PRODUCT_ASSET_TAG',
            id: productId.assetTag,
            content: '',
            editable: this.showEditIcon,
            maxSize: 48,
            controll: new UntypedFormControl(''),
            validationRules: [this.validator.tagValid],
            confirm: (value) => {
                value = value.replace(/\n/g, '');
                const self = this.productInfos.find((item) => item.id === productId.assetTag);
                self.controll.setValue(value);
                this.productServ.updateAssetTag(value).subscribe(() => {
                    this.alertServ.eventEmit.emit({
                        type: 'success',
                        label: 'COMMON_SUCCESS'
                    });
                    self.content = value;
                }, () => {
                    self.controll.setValue(self.content);
                });
            },
            validation: {
                tip: formatEntry(this.translate.instant('COMMON_TIPS_NOCHECK'), [0, 48]),
                tipPosition: 'top-left'
            },
        },
        {
            label: 'INFO_PRODUCT_POSITION',
            id: productId.position,
            content: '',
            editable: this.showEditIcon,
            maxSize: 64,
            controll: new UntypedFormControl(''),
            validationRules: [this.validator.positionValid],
            confirm: (value) => {
                const self = this.productInfos.find((item) => item.id === productId.position);
                this.productServ.updateDeviceLocation(value).subscribe(() => {
                    this.alertServ.eventEmit.emit({
                        type: 'success',
                        label: 'COMMON_SUCCESS'
                    });
                    self.content = value;
                }, () => {
                    self.controll.setValue(self.content);
                });
            },
            validation: {
                tip: formatEntry(this.translate.instant('COMMON_TIPS_NOCHECK'), [0, 64]),
                tipPosition: 'top-left'
            },
        },
        {
            label: 'INFO_MAIN_BOARD_BIOS_VERSION',
            id: mainBoardId.biosVersion,
            hide: true,
            content: '',
        },
        {
            label: 'INFO_MAIN_BOARD_IBMC_VERSION',
            id: mainBoardId.ibmcVersion,
            hide: true,
            content: '',
        },
        {
            label: 'INFO_MAIN_BOARD_MAIN_BOOT',
            id: mainBoardId.mainBoot,
            hide: true,
            content: '',
        },
        {
            label: 'INFO_MAIN_BOARD_BACKUP_BOOT',
            id: mainBoardId.backupBoot,
            hide: true,
            content: '',
        },
    ];

    // 主板信息
    public mainBoardInfos: IItem[] = [
        {
            label: 'INFO_MAIN_BOARD_IBMC_VERSION',
            id: mainBoardId.ibmcVersion,
            hide: false,
            content: '',
        },
        {
            label: 'OTHER_BOARD_ID',
            id: mainBoardId.boardId,
            hide: false,
            content: '',
        },
        {
            label: 'INFO_MAIN_BOARD_BIOS_VERSION',
            id: mainBoardId.biosVersion,
            hide: false,
            content: '',
        },
        {
            label: 'INFO_MAIN_BOARD_MANUFACTURER',
            id: mainBoardId.boardManu,
            hide: false,
            content: '',
        },
        {
            label: 'COMMIN_CPLD_VERSION',
            id: mainBoardId.cpldVersion,
            hide: false,
            content: '',
        },
        {
            label: 'INFO_MAIN_BOARD_VERSION',
            id: mainBoardId.boardVersion,
            hide: false,
            content: '',
        },
        {
            label: 'INFO_MAIN_BOARD_MAIN_BOOT',
            id: mainBoardId.mainBoot,
            hide: false,
            content: '',
        },
        {
            label: 'INFO_MAIN_BOARD_SERIAL',
            id: mainBoardId.serial,
            hide: false,
            content: '',
        },
        {
            label: 'INFO_MAIN_BOARD_BACKUP_BOOT',
            id: mainBoardId.backupBoot,
            hide: false,
            content: '',
        },
        {
            label: 'INFO_MAIN_BOARD_PCH_VERSION',
            id: mainBoardId.pchVersion,
            hide: false,
            content: '',
        },
        {
            label: 'INFO_MAIN_BOARD_CHASIS_ID',
            id: mainBoardId.chasisId,
            content: '',
            hide: false,
        },
        {
            label: 'INFO_MAIN_BOARD_SLOT',
            id: mainBoardId.deviceSlotID,
            hide: false,
            content: '',
        },
        {
            label: 'INFO_MAIN_BOARD_PCB_VERSION',
            id: mainBoardId.pcbVersion,
            hide: false,
            content: '',
        },
        {
            label: 'OTHER_PART_NUMBER',
            id: mainBoardId.partNumber,
            hide: false,
            content: '',
        },
    ];

    // 系统软件
    public systemInfos: IItem[] = [
        {
            label: 'INFO_SYSTEM_LOCAL_DOMAIN',
            id: systemId.localDomain,
            content: '',
        },
        {
            label: 'INFO_SYSTEM_IBMA_SERVICE',
            id: systemId.ibmaService,
            content: '',
        },
        {
            label: 'INFO_SYSTEM_HOST_DESCRIPTION',
            id: systemId.description,
            content: '',
        },
        {
            label: 'INFO_SYSTEM_IBMA_STATUS',
            id: systemId.ibmaStatus,
            content: '',
        },
        {
            label: 'INFO_SYSTEM_KERNAL_VERSION',
            id: systemId.kernalVersion,
            content: '',
        },
        {
            label: 'INFO_SYSTEM_IBMA_DRIVER',
            id: systemId.ibmaDriver,
            content: '',
        },
        {
            label: 'INFO_SYSTEM_DOMAIN',
            id: systemId.domain,
            content: '',
        },
        {
            label: 'INFO_SYSTEM_OPERATOR',
            id: systemId.operator,
            content: '',
        },
    ];

    // 电子保单
    public digitalWarrantyInfos: IItem[] = [
        {
            label: 'INFO_WARRANTY_NAME',
            id: DigitalWarrantyId.productNamePolicy,
            content: ''
        },
        {
            label: 'INFO_PRODUCT_SERIAL_NUMBER',
            id: DigitalWarrantyId.serialNumber,
            content: ''
        },
        {
            label: 'INFO_WARRANTY_MANUFACTURE_DATE',
            id: DigitalWarrantyId.manufactureDate,
            content: '',
        },
        {
            label: 'INFO_WARRANTY_UUID',
            id: DigitalWarrantyId.UUID,
            content: '',
        },
        {
            label: 'INFO_WARRANTY_TYPE',
            id: DigitalWarrantyId.unitType,
            content: ''
        },
        {
            label: 'INFO_WARRANTY_START_POINT',
            id: DigitalWarrantyId.startPoint,
            content: '',
            editableDate: this.showEditIcon,
            format: 'yyyy-MM-dd'
        },
        {
            label: 'INFO_WARRANTY_LIFE_span',
            id: DigitalWarrantyId.lifespan,
            content: '',
            editable: this.showEditIcon,
            controll: new UntypedFormControl(''),
            validationRules: [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 255)],
            confirm: (value) => {
                const self = this.digitalWarrantyInfos.find((item) => item.id === DigitalWarrantyId.lifespan);
                this.productServ.updateWarrantySpan(value).subscribe(() => {
                    this.alertServ.eventEmit.emit({
                        type: 'success',
                        label: 'COMMON_SUCCESS'
                    });
                    self.content = value;
                }, () => {
                    self.controll.setValue(self.content);
                });
            }
        },
    ];

    // 是否隐藏电子保单标志位
    public hideDigitalWarranty: boolean = false;

    // 电子保单日期编辑
    public isShowEdit = false;
    public startPointValue: null;
    public editDate(value) {
        this.isShowEdit = true;
        const splitDatePre = value.split('-');
        this.startPointValue = new Date(Date.UTC(Number(splitDatePre[0]), Number(splitDatePre[1]) - 1, Number(splitDatePre[2])));
    }
    public editDateConfirm(value) {
        if (!value) {
            return;
        }
        value = this._dateToStr(value);
        const self = this.digitalWarrantyInfos.find((item) => item.id === DigitalWarrantyId.startPoint);
        this.productServ.updateWarrantyDate(value).subscribe(() => {
            this.alertServ.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS'
            });
            this.isShowEdit = false;
            self.content = value;
        }, () => {
            this.isShowEdit = false;
        });
    }
    public editDateCancel() {
        this.isShowEdit = false;
    }
    private _dateToStr(date) {
        const y = date.getFullYear();
        let m = (date.getMonth() + 1).toString();
        let d = date.getDate();
        if (m < 10) {
            m = `0${m}`;
        }
        if (d < 10) {
            d = `0${d}`;
        }
        return `${y}-${m}-${d}`;
    }
    private initInfo(infoModel: IItem[], productInfo: IInfo) {
        for (const item of infoModel) {
            const info = productInfo[item.id];
            if (info === undefined) {
                item.hide = true;
                continue;
            }
            if (typeof (info) === 'string' || typeof (info) === 'number') {
                if (item.controll) {
                    item.controll.setValue(info);
                }
                item.content = info;
            } else {
                item.content = info.value;
                if (info.value === null) {
                    info.hide = true;
                }
                item.hide = info.hide;
                if (item.id === 'lifespan') {
                    item.content = info.value + '';
                    item.controll.setValue(item.content);
                }
            }
        }
        this.showSmsEnabled = productInfo.showSmsEnabled;
        this.showSysInfo = productInfo.showSysInfo;
    }

    constructor(
        private http: HttpService,
        private productServ: ProductService,
        private loadingService: LoadingService,
        private translate: TranslateService,
        private alertServ: AlertMessageService,
        private userServ: UserInfoService,
        private commonService: CommonService,
        private globalData: GlobalDataService
    ) {
        if (this.translate.defaultLang === 'zh-CN') {
            this.labelWidth = '152px';
        }
        if (this.translate.defaultLang === 'en-US') {
            this.labelWidth = '250px';
        }
        if (this.translate.defaultLang === 'ja-JA') {
            this.labelWidth = '270px';
        }
        if (this.translate.defaultLang === 'fr-FR') {
            this.labelWidth = '265px';
        }
    }

    ngOnInit() {
        // 是否显示系统软件,根据接口字段来实现
        this.globalData.ibmaSupport.then((res: boolean) => {
            this.showSysInfoState = res;
        });
        this.loadingService.state.next(true);
        this.queryInfoSub = this.productServ.getData().subscribe((productInfo: IInfo) => {
            this.initInfo([...this.productInfos, ...this.mainBoardInfos, ...this.systemInfos, ...this.digitalWarrantyInfos], productInfo);
            if (productInfo?.lifespan?.value === null) {
                this.hideDigitalWarranty = true;
            }
            // 产品序列号和系统序列号一致时隐藏系统序列号
            const productSerial = this.productInfos.find(item => item.id === productId.productSerialNum);
            const systemSerial = this.productInfos.find(item => item.id === productId.systemSN);
            if (productSerial.content !== systemSerial.content) {
                systemSerial.hide = false;
            }
            const hideList: string[] = [mainBoardId.biosVersion, mainBoardId.ibmcVersion, mainBoardId.mainBoot, mainBoardId.backupBoot];
            if (this.globalData.productType === PRODUCTTYPE.TIANCHI) {
                this.productInfos.forEach(item => {
                    if (hideList.includes(item.id)) {
                        item.hide = false;
                    }
                });
                this.isTianchiBoard = true;
                const bomNumberInfo = this.productInfos.find(item => item.id === productId.bomNumber);
                bomNumberInfo.label = 'INFO_PRODUCT_NUMBER';
            } else if (this.globalData.productType === PRODUCTTYPE.WUHAN) {
                this.mianBoardTitle = this.translate.instant('INFO_EXPANSION_BOARD');
                this.mainBoardInfos[1].label = 'EXPANSION_BOARD_ID';
                this.mainBoardInfos[3].label = 'EXPANSION_BOARD_MANUFACTURER';
                this.mainBoardInfos[5].label = 'INFO_EXPANSION_BOARD_VERSION';
                this.mainBoardInfos[7].label = 'INFO_EXPANSION_BOARD_SERIAL';
                // 武汉二期隐藏“电子保单”
                this.hideDigitalWarranty = true;
            } else if (this.globalData.productType === PRODUCTTYPE.ATLAS) {
                const showList: string[] = [mainBoardId.ibmcVersion, mainBoardId.cpldVersion,
                    mainBoardId.mainBoot, mainBoardId.backupBoot, mainBoardId.deviceSlotID];
                this.mainBoardInfos.forEach(item => {
                    if (showList.includes(item.id) && item.hide === false) {
                        item.hide = false;
                    } else {
                        item.hide = true;
                    }
                });
                this.isAtlas1951 = true;
                this.mianBoardTitle = this.translate.instant('INFO_BMC_BOARD');
            } else {
                this.mianBoardTitle = this.translate.instant('INFO_MAIN_BOARD');
            }
            this.loadingService.state.next(false);
            this.loaded = true;
        });
        }

    ngOnDestroy(): void {
        this.queryInfoSub.unsubscribe();
    }

}
