import { TranslateService } from '@ngx-translate/core';
import { Component, OnInit } from '@angular/core';
import { Subscription } from 'rxjs';
import { ProductService } from './service';
import { IItem, IInfo, CabinetInfoId, BaseInfoId } from './product.datatype';
import { UntypedFormControl, UntypedFormGroup, UntypedFormBuilder } from '@angular/forms';
import Validator from './validator';
import { formatEntry, getMessageId, MultVaild } from 'src/app/common-app/utils';
import { TiValidators, TiValidationConfig } from '@cloud/tiny3';
import {
    ErrortipService,
    AlertMessageService,
    LoadingService,
    UserInfoService
} from 'src/app/common-app/service';
import { CommonData } from 'src/app/common-app/models';

@Component({
    selector: 'app-product',
    templateUrl: './product.component.html',
    styleUrls: ['./product.component.scss']
})
export class ProductComponent implements OnInit {

    constructor(
        private productServ: ProductService,
        private loadingService: LoadingService,
        private translate: TranslateService,
        private alertServ: AlertMessageService,
        private userServ: UserInfoService,
        private fb: UntypedFormBuilder,
        private errorTip: ErrortipService
    ) {
        if (this.translate.defaultLang === 'zh-CN') {
            this.labelWidth = '152px';
        } else if (this.translate.defaultLang === 'en-US') {
            this.labelWidth = '228px';
        } else if (this.translate.defaultLang === 'ja-JA') {
            this.labelWidth = '230px';
        } else if (this.translate.defaultLang === 'fr-FR') {
            this.labelWidth = '265px';
        } else if (this.translate.defaultLang === 'ru-RU') {
            this.labelWidth = '270px';
        }
    }

    public colsNumber = 2;
    public colsNumberIrm = 1;

    public verticalAlign = 'middle';

    public labelWidth = '';

    public colsGap = ['200px'];

    public showEditIcon = this.userServ.hasPrivileges(['ConfigureComponents']);

    public systemLockDownEnabled = this.userServ.systemLockDownEnabled;

    private queryInfoSub: Subscription;

    public loaded = false;
    public cabinetEdit: boolean = false;
    public cabinetEditForm: UntypedFormGroup;
    public cabinetDisabled: boolean = true;
    private changeData: { [key: string]: any } = {};
    private baseCabinetFormData: { [key: string]: any } = {};

    private i18n = {
        COMMON_FORMAT_ERROR: this.translate.instant('COMMON_FORMAT_ERROR'),
    };

    private validator = new Validator(this.i18n);
    // 基本信息
    public baseInfos: IItem[] = [
        {
            label: 'IRM_INFO_MODEL',
            id: BaseInfoId.productModel,
            content: ''
        },
        {
            label: 'INFO_PRODUCT_ASSET_TAG',
            id: BaseInfoId.productTag,
            content: '',
            editable: this.showEditIcon,
            maxSize: 48,
            controll: new UntypedFormControl(''),
            validationRules: [this.validator.tagValid],
            confirm: (value) => {
                const self = this.baseInfos.find((item) => item.id === BaseInfoId.productTag);
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
            label: 'COMMON_SERIALNUMBER',
            id: BaseInfoId.serialNumberBase,
            content: ''
        },
        {
            label: 'IRM_PART_NUMBER',
            id: BaseInfoId.partNumberBase,
            content: ''
        },
        {
            label: 'COMMON_MANUFACTURER',
            id: BaseInfoId.vendor,
            content: ''
        },
        {
            label: 'COMMON_FIXED_VERSION',
            id: BaseInfoId.firmwareVersion,
            content: ''
        },
        {
            label: 'OTHER_BOARD_ID',
            id: BaseInfoId.veneerID,
            content: ''
        },
    ];
    // 机柜信息
    public cabinetInfos: IItem[] = [
        {
            label: 'IRM_POSITION',
            id: CabinetInfoId.cabinetPosition,
            content: '',
        },
        {
            label: this.translate.instant('ASSET_RATED_POWER_WATTS'),
            id: CabinetInfoId.ratedPower,
            content: '',
        },
        {
            label: 'IRM_BEARING_CAPACITY',
            id: CabinetInfoId.loadCapacityKg,
            content: '',
        },
        {
            label: 'IRM_HOME_TOTAL',
            id: CabinetInfoId.totalUCount,
            content: '',
        },
        {
            label: this.translate.instant('IRM_HOME_RACK_USE') + '(%)',
            id: CabinetInfoId.cabinetUse,
            content: '',
        }
    ];
    public cabinetNameValidator: TiValidationConfig = {
        tip: formatEntry(this.translate.instant('VALIDATOR_PWD_VALIDATOR1'), [1, 48]),
        tipPosition: 'right',
        errorMessage: {
            pattern: this.translate.instant('COMMON_FORMAT_ERROR'),
            notAllWhiteSpace: this.translate.instant('COMMON_FORMAT_ERROR')
        }
    };
    public cabinetRowValidator: TiValidationConfig = {
        tip: formatEntry(this.translate.instant('IRM_INFO_VALUE_RANGE_TIP'), [0, 500]),
        tipPosition: 'right',
        errorMessage: {
            pattern: this.translate.instant('COMMON_FORMAT_ERROR')
        }
    };
    public ratedPowerValidator: TiValidationConfig = {
        type: 'blur',
        tip: formatEntry(this.translate.instant('IRM_INFO_VALUE_RANGE_TIP'), [1, 2147483647]),
        tipPosition: 'right',
        errorMessage: {
            pattern: this.translate.instant('COMMON_FORMAT_ERROR')
        }
    };
    private initInfo(infoModel: IItem[], productInfo: IInfo) {
        for (const item of infoModel) {
            const info = productInfo[item.id];
            if (info === undefined) {
                item.hide = true;
                continue;
            }
            if (typeof (info) === 'string') {
                if (item.controll) {
                    item.controll.setValue(info);
                }
                item.content = info;
            } else if (typeof (info) === 'number') {
                item.content = info + '';
            }
        }
        this.editData(productInfo);
    }
    // 机柜信息
    public cabinetInfoEdit() {
        this.cabinetEdit = true;
    }
    public cabinetCancel() {
        this.getInfo();
        this.cabinetEdit = false;
        this.cabinetDisabled = true;
    }
    // 绑定表单的变更事件
    private bindAssetFormControlChange() {
        const controls = this.cabinetEditForm.controls;
        Object.keys(this.cabinetEditForm.controls).forEach((controlName) => {
            controls[controlName].valueChanges.subscribe((value) => {
                if (controlName === CabinetInfoId.ratedPower ||
                    controlName === CabinetInfoId.loadCapacityKg) {
                    value = parseInt(value, 10);
                }
                if (this.baseCabinetFormData[controlName] !== value) {
                    this.changeData[controlName] = true;
                } else {
                    delete this.changeData[controlName];
                }
                setTimeout(() => {
                    this.computeState();
                }, 10);
            });
        });
    }
    // 计算保存按钮的状态
    private computeState() {
        // 1. 首先是计算表格表单元素数据是否变化或校验通过
        const formChanged = Object.keys(this.changeData).length > 0;
        const valid = this.cabinetEditForm.valid;
        // 按钮禁用的条件：  1. 表单 2.表单校验出错。
        this.cabinetDisabled = !formChanged || !valid;
        return this.cabinetDisabled;
    }
    public cabinetSave() {
        const param: { [key: string]: any } = {};
        // 组合form表单的参数
        Object.keys(this.changeData).forEach((controlName) => {
            const value = this.cabinetEditForm.controls[controlName].value;
            param[controlName] = value;
            if (controlName === CabinetInfoId.ratedPower ||
                controlName === CabinetInfoId.loadCapacityKg) {
                param[controlName] = parseInt(param[controlName], 10);
            }
            if (controlName === CabinetInfoId.cabinetRack ||
                controlName === CabinetInfoId.cabinetRow) {
                param[controlName] = parseInt(param[controlName], 10) + '';
            }
            this.loadingService.state.next(true);
        });
        this.productServ.cabinetSave(param).subscribe({
            next: (res) => {
                this.alertServ.eventEmit.emit({
                    type: 'success',
                    label: 'COMMON_SUCCESS'
                });
                this.cabinetEdit = false;
                this.cabinetDisabled = true;
                this.getInfo();
                this.loadingService.state.next(false);
            },
            error: (error) => {
                this.loadingService.state.next(false);
                const errorMessage = this.errorTip.getErrorMessage(getMessageId(error.error)[0].errorId) || 'COMMON_FAILED';
                this.alertServ.eventEmit.emit({ type: 'error', label: errorMessage });
            }
        });
    }
    public editData(productInfo) {
        Object.keys(productInfo).forEach((index) => {
            if (productInfo[index] === CommonData.isEmpty) {
                productInfo[index] = '';
            }
        });
        this.cabinetEditForm = this.fb.group({
            'Name': ['', [TiValidators.required, TiValidators.rangeSize(1, 48), MultVaild.notAllWhiteSpace()]],
            'Row': ['', [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(0, 500)]],
            'Rack': ['', [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(0, 500)]],
            'RatedPowerWatts': ['', [TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 2147483647)]],
            'LoadCapacityKg': ['', [TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 2147483647)]],
            'TotalUCount': [''],
            'UcountUsed': [''],
        });
        this.cabinetEditForm.patchValue({
            'Name': productInfo['Name'],
            'Row': productInfo['Row'],
            'Rack': productInfo['Rack'],
            'RatedPowerWatts': productInfo['RatedPowerWatts'],
            'LoadCapacityKg': productInfo['LoadCapacityKg'],
            'TotalUCount': productInfo['TotalUCount'],
            'UcountUsed': productInfo['UcountUsed'],
        });
        this.cabinetEditForm.controls['TotalUCount'].disable();
        this.cabinetEditForm.controls['UcountUsed'].disable();
        // 保存表单基础数据
        this.baseCabinetFormData = JSON.parse(JSON.stringify(this.cabinetEditForm.value));
        this.bindAssetFormControlChange();
    }
    ngOnInit() {
        this.getInfo();
    }
    public getInfo() {
        this.loadingService.state.next(true);
        this.changeData = {};
        this.queryInfoSub = this.productServ.getData().subscribe((productInfo: IInfo) => {
            const paramArr = [
                ...this.cabinetInfos,
                ...this.baseInfos
            ];
            this.initInfo(paramArr, productInfo);
            this.loadingService.state.next(false);
            this.loaded = true;
        });
    }

    ngOnDestroy(): void {
        this.queryInfoSub.unsubscribe();
    }

}
