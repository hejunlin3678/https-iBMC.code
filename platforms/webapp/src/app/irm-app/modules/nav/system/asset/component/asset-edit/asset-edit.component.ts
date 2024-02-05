import { Component, OnInit, Output, Input, EventEmitter } from '@angular/core';
import { AssetService } from '../../services/asset.service';
import { TranslateService } from '@ngx-translate/core';
import { TiValidationConfig, TiValidators } from '@cloud/tiny3';
import { getMessageId, formatEntry } from 'src/app/common-app/utils';
import { getPatternTip, ASSET_SPECIAL_CHAR, ASSET_SPECIAL_CHAR_REQUIRE, ASSET_SPECIAL_EMPTY } from '../../utils/asset.utils';
import { AssetEditFormat } from '../../model/asset.datatype';
import { CommonData } from 'src/app/common-app/models';
import {
    LoadingService,
    AlertMessageService,
    ErrortipService
} from 'src/app/common-app/service';

// form组件,校验
import { UntypedFormBuilder, UntypedFormControl, UntypedFormGroup, Validators } from '@angular/forms';
import { AssetValid } from '../../valid/asset.valid';

@Component({
    selector: 'app-asset-edit',
    templateUrl: './asset-edit.component.html',
    styleUrls: ['./asset-edit.component.scss']
})
export class AssetEditComponent implements OnInit {

    constructor(
        private assetService: AssetService,
        private translate: TranslateService,
        private loading: LoadingService,
        private alertService: AlertMessageService,
        private fb: UntypedFormBuilder,
        private errorTip: ErrortipService
    ) {

    }
    @Input() editData;
    @Input() halfTitle: string;
    @Input() isSystemLock: boolean;
    @Output() close: EventEmitter<any> = new EventEmitter<any>();
    @Output() reInit: EventEmitter<any> = new EventEmitter<any>();
    // 响应式表单类型定义
    public assetEditForm: UntypedFormGroup;
    public customField: boolean = true;
    public dateFormat: string = 'yyyy/MM/dd';
    public labelWidth = '152px';
    public checkInTime = null;
    public dateChange: boolean = true;
    // 原始数据保存
    public saveBtnStatus: boolean = true;
    private baseAssetFormData: { [key: string]: any } = {};
    private changeData: { [key: string]: any } = {};
    /**
     * 输入校验相关配置信息，注：校验规则不是在这里定义，校验规则在fb.group中定义，后续对校验规则的修改，需要用到 formControl的setValidators方法
     */
    public patternValidator: TiValidationConfig = getPatternTip(this.translate.instant('COMMON_FORMAT_ERROR'), this.translate.instant('ASSET_IRM_REQUIRED'));

    public uHeightValidator: TiValidationConfig = getPatternTip(this.translate.instant('ASSET_HEIGHT_REQUIRED'), this.translate.instant('ASSET_HEIGHT_REQUIRED'));

    public lifeCircleValidator: TiValidationConfig = getPatternTip(this.translate.instant('ASSET_LIFE_CYCLE_RANGE'), this.translate.instant('ASSET_LIFE_CYCLE_RANGE'));

    public weightValidator: TiValidationConfig = getPatternTip(this.translate.instant('ASSET_WEIGHT_RANGE'), this.translate.instant('ASSET_WEIGHT_RANGE'));

    public ownerValidator: TiValidationConfig = getPatternTip(this.translate.instant('COMMON_FORMAT_ERROR'), this.translate.instant('ASSET_MIN_REQUIRED'));

    public typOptions = [
        { id: 'Server', label: 'Server' },
        { id: 'Storage', label: 'Storage' },
        { id: 'Network', label: 'Network' }
    ];
    public extendField;
    public extendFieldInit = [];
    // 自定义字段的key值校验方式
    public validationValue: TiValidationConfig = {
        type: 'change',
        errorMessage: {
            pattern: this.translate.instant('COMMON_FORMAT_ERROR')
        },
        tipPosition: 'right',
        tip: this.translate.instant('ASSET_MIN_REQUIRED'),
    };
    public validationKey: TiValidationConfig = {
        tipPosition: 'right',
        type: 'change',
        errorMessage: {
            pattern: this.translate.instant('ASSET_LETTER'),
            patternRepeat: this.translate.instant('ASSET_REPEAT')
        },
        tip: this.translate.instant('ASSET_KEY_REQUIRED'),
    };

    // 添加自定义字段
    public addInput() {
        const extend = this.extendField;
        let valid = false;
        if (extend.length > 0) {
            Object.keys(extend).forEach((index) => {
                if (extend[index].keyControl.errors || extend[index].valueControl.errors) {
                    valid = true;
                }
            });
        }
        if (extend.length >= 5 || valid) {
            return;
        }
        const temp = { key: '', value: '' };
        extend.push(temp);
        this.extendValid(extend, '');
    }
    public extendChange(data, keyValue) {
        this.extendValid(data, keyValue);
        this.assetChange();
    }
    public extendChangeValue() {
        this.assetChange();
    }
    // 添加自定义字段的校验
    public extendValid(validData, keyValue) {
        validData.forEach(item => {
            item.keyControl = new UntypedFormControl(
                item.key,
                [TiValidators.required, Validators.pattern(/^[a-zA-Z0-9]+$/), AssetValid.getRepeat(validData, keyValue)]
            );
            item.valueControl = new UntypedFormControl(
                item.value,
                [Validators.pattern(ASSET_SPECIAL_CHAR)]
            );
        });
    }
    // 删除扩展字段
    public delExtend(key) {
        this.extendField = this.extendField.filter(item => {
            return item.key !== key;
        });
        this.assetChange();
    }
    ngOnInit() {
        Object.keys(this.editData).forEach((index) => {
            if (this.editData[index] === CommonData.isEmpty) {
                this.editData[index] = '';
            }
        });
        this.editData.extendField.forEach(item => {
            if (item.value === CommonData.isEmpty) {
                item.value = '';
            }
        });
        this.assetEditForm = this.fb.group({
            'Model': ['', [TiValidators.required, Validators.pattern(ASSET_SPECIAL_CHAR_REQUIRE), Validators.pattern(ASSET_SPECIAL_EMPTY)]],
            'DeviceType': ['',
                [TiValidators.required, Validators.pattern(ASSET_SPECIAL_CHAR_REQUIRE), Validators.pattern(ASSET_SPECIAL_EMPTY)]],
            'SerialNumber': ['',
                [TiValidators.required, Validators.pattern(ASSET_SPECIAL_CHAR_REQUIRE), Validators.pattern(ASSET_SPECIAL_EMPTY)]],
            'UHeight': ['', [TiValidators.required, TiValidators.number, TiValidators.integer,
            TiValidators.minValue(1), TiValidators.maxValue(this.editData.availableRackSpaceU)]],
            'Manufacturer': ['',
                [TiValidators.required, Validators.pattern(ASSET_SPECIAL_CHAR_REQUIRE), Validators.pattern(ASSET_SPECIAL_EMPTY)]],
            'PartNumber': ['', [Validators.pattern(ASSET_SPECIAL_CHAR)]],
            'LifeCycleYear': ['', [TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 255)]],
            'WeightKg': ['', [TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 2147483647)]],
            'RatedPowerWatts': ['', [TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 2147483647)]],
            'AssetOwner': ['', [Validators.pattern(ASSET_SPECIAL_CHAR)]],
            'CheckInTime': ['']
        });
        this.assetEditForm.patchValue({
            'Model': this.editData.pattern,
            'DeviceType': this.editData.deviceType,
            'SerialNumber': this.editData.serialNumber,
            'UHeight': this.editData.uHeight,
            'Manufacturer': this.editData.factory,
            'PartNumber': this.editData.partNumber,
            'LifeCycleYear': this.editData.lifeCircle,
            'WeightKg': this.editData.weight,
            'RatedPowerWatts': this.editData.ratedPower,
            'AssetOwner': this.editData.owner,
            'CheckInTime': new Date(this.editData.startTime)
        });
        this.checkInTime = new Date(this.editData.startTime);
        this.extendValid(this.editData.extendField, '');
        this.extendField = this.editData.extendField;
        if (this.editData.extendField) {
            this.extendFieldInit = JSON.parse(JSON.stringify(this.editData.extendField));
        }
        // 保存表单基础数据
        this.baseAssetFormData = JSON.parse(JSON.stringify(this.assetEditForm.value));
        this.bindAssetFormControlChange();
        this.uHeightValidator = getPatternTip(
            formatEntry(this.translate.instant('ASSET_HEIGHT_REQUIRED'), [this.editData.availableRackSpaceU]),
            formatEntry(this.translate.instant('ASSET_HEIGHT_REQUIRED'), [this.editData.availableRackSpaceU]));
    }
    // 绑定表单的变更事件
    private bindAssetFormControlChange() {
        const controls = this.assetEditForm.controls;
        Object.keys(this.assetEditForm.controls).forEach((controlName) => {
            controls[controlName].valueChanges.subscribe((value) => {
                if (controlName === AssetEditFormat.UHEIGHT ||
                    controlName === AssetEditFormat.WEIGHTKG ||
                    controlName === AssetEditFormat.LIFECYCLEYEAR ||
                    controlName === AssetEditFormat.RATEDPOWERWATTS) {
                    value = parseInt(value, 10);
                }
                if (this.baseAssetFormData[controlName] !== value) {
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
    // 保存按钮状态
    private assetChange() {
        let valid = false;
        Object.keys(this.extendField).forEach((index) => {
            if (this.extendField[index].keyControl.errors || this.extendField[index].valueControl.errors) {
                valid = true;
                this.saveBtnStatus = true;
            }
        });
        if (valid) {
            return;
        }
        this.customField = true;
        if (this.extendField.length === this.extendFieldInit.length) {
            Object.keys(this.extendFieldInit).forEach((index) => {
                if (this.extendFieldInit[index].key !== this.extendField[index].key
                    || this.extendFieldInit[index].value !== this.extendField[index].valueControl.value) {
                    this.customField = false;
                }
            });
        } else {
            this.customField = false;
        }
        this.computeState();
    }
    // 计算保存按钮的状态
    private computeState() {
        // 1. 首先是计算表格表单元素数据是否变化或校验通过
        const formChanged = Object.keys(this.changeData).length > 0;
        const valid = this.assetEditForm.valid;
        // 按钮禁用的条件：  1. 表单 2.表单校验出错。
        this.saveBtnStatus = (!formChanged && this.customField && this.dateChange) || !valid;
        return this.saveBtnStatus;
    }
    // 时间格式处理
    private _dateToStr(date) {
        const year = date.getFullYear();
        let month = date.getMonth() + 1;
        let day = date.getDate();
        if (month < 10) {
            month = `0${month}`;
        }
        if (day < 10) {
            day = `0${day}`;
        }
        return `${year}/${month}/${day}`;
    }
    // 保存资产编辑数据
    public save() {
        if (this.extendField.length > 5 || this.isSystemLock) {
            return;
        }
        const param: { [key: string]: any } = {};
        // 组合form表单的参数
        Object.keys(this.assetEditForm.value).forEach((controlName) => {
            let value = this.assetEditForm.controls[controlName].value;
            if (controlName === AssetEditFormat.CHECKINTIIME) {
                if (this.checkInTime) {
                    value = this._dateToStr(this.checkInTime);
                } else {
                    value = null;
                }
            }
            if (value && typeof (value) === 'string') {
                value = value.trim();
            }
            param[controlName] = value;
            if (controlName === AssetEditFormat.UHEIGHT ||
                controlName === AssetEditFormat.WEIGHTKG ||
                controlName === AssetEditFormat.LIFECYCLEYEAR ||
                controlName === AssetEditFormat.RATEDPOWERWATTS) {
                param[controlName] = parseInt(param[controlName], 10);
            }
        });
        // 添加扩展字段
        const extendField = {};
        this.extendField.forEach((item) => {
            if (item.value) {
                item.value = item.value.trim();
            }
            extendField[item.key] = item.value;
        });
        param['ExtendField'] = extendField;
        this.loading.state.next(true);
        this.assetService.saveAssetTag(this.editData.uNum, param).subscribe({
            next: (res) => {
                this.alertService.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                this.loading.state.next(false);
                this.closeModal();
            },
            error: (error) => {
                this.loading.state.next(false);
                const errorMessage = this.errorTip.getErrorMessage(getMessageId(error.error)[0].errorId) || 'COMMON_FAILED';
                this.alertService.eventEmit.emit({ type: 'error', label: errorMessage });
            }
        });
    }
    // 首次使用时间改变判断
    public checkInTimeChange(value) {
        if (value) {
            value = this._dateToStr(value);
        } else {
            this.checkInTime = null;
        }
        let baseTime = null;
        if (this.baseAssetFormData[AssetEditFormat.CHECKINTIIME]) {
            baseTime = this._dateToStr(new Date(this.baseAssetFormData[AssetEditFormat.CHECKINTIIME]));
        }
        this.dateChange = (value === baseTime);
        this.computeState();
    }
    // 关闭半屏弹窗
    public closeModal() {
        this.close.next(true);
    }
}
