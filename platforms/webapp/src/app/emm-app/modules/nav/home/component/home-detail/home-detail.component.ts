import { Component, OnInit, Input } from '@angular/core';
import { SystemLockService } from 'src/app/common-app/service/systemLock.service';
import { HomeService } from '../../services';
import { AlertMessageService, LoadingService } from 'src/app/common-app/service';
import { ProductName } from 'src/app/emm-app/models/common.datatype';
import { CommonData } from 'src/app/common-app/models';
import { UntypedFormControl } from '@angular/forms';
import { TiValidationConfig, TiValidators } from '@cloud/tiny3';
import { ChassisVaild } from './vaild/chassis.vaild';
import { formatEntry } from 'src/app/common-app/utils';
import { TranslateService } from '@ngx-translate/core';

@Component({
    selector: 'app-home-emm-detail',
    templateUrl: './home-detail.component.html',
    styleUrls: ['./home-detail.component.scss'],
})
export class HomeEmmDetailComponent implements OnInit {

    @Input() serverDetail;

    // 获取产品数据的订阅者
    public reader;

    // 产品Model 'TCE8080','TCE8040','E9000'
    public model = '';
    public productData;

    public modelList = ProductName;

    public colsNumber = 2;

    public systemLocked: boolean = true;

    public colsGap = ['100px'];

    public isEditChassisInfo: boolean = false;
    public nameControl: UntypedFormControl;
    public locationControl: UntypedFormControl;
    public idControl: UntypedFormControl;

    constructor(
        private lockService: SystemLockService,
        private service: HomeService,
        private loadingService: LoadingService,
        private alert: AlertMessageService,
        private translate: TranslateService

    ) {
        // 添加订阅者 获取数据
        this.reader = this.service.subjectObj.subscribe((data: any) => {
            if (data ?.data ?.Chasis ?.Model) {
                this.productData = data.data;
                // 当前可能的值为 E9000  TCE8040  TCE8080
                this.model = data.data.Chasis.Model;
            }
        });
    }

    ngOnInit() {
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.systemLocked !== res) {
                this.systemLocked = res;
            }
        });
    }

    // 编辑机框信息
    public editDetail() {
         // 名称校验
        this.nameControl = new UntypedFormControl(this.serverDetail[0].value,  [ChassisVaild.bytesLength()]);
        // 位置校验
        this.locationControl = new UntypedFormControl(
            this.serverDetail[2].value, [ChassisVaild.bytesLength(), ChassisVaild.noChineseCharacters()]
        );
        // id校验
        this.idControl = new UntypedFormControl(
            this.serverDetail[4].value,
            [TiValidators.required, TiValidators.number, TiValidators.rangeValue(0, 999999)]
        );
        const validationName: TiValidationConfig = {
            type: 'blur',
            errorMessage: {
                'bytesLength': this.translate.instant('VALID_BYTE_LENGTH')
            },
        };
        const validationLocation: TiValidationConfig = {
            type: 'blur',
            errorMessage: {
                'bytesLength': this.translate.instant('VALID_BYTE_LENGTH'),
                'noChineseCharacters': this.translate.instant('VALID_CHASSIS_POSITION_TIP')
            }
        };
        const validationId: TiValidationConfig = {
            type: 'blur',
            errorMessage: {
                required: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [0, 999999]),
                number: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [0, 999999]),
                rangeValue: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [0, 999999])
            }
        };
        this.serverDetail[0].formControl = this.nameControl,
        this.serverDetail[2].formControl = this.locationControl,
        this.serverDetail[4].formControl = this.idControl,

        this.serverDetail[0].validation = validationName,
        this.serverDetail[2].validation = validationLocation,
        this.serverDetail[4].validation = validationId,

        this.serverDetail[0].isEdit = true;
        this.serverDetail[2].isEdit = true;
        this.serverDetail[4].isEdit = true;
        this.isEditChassisInfo = true;
    }

    public editCancel(status: boolean = false) {
        this.serverDetail[0].isEdit = false;
        this.serverDetail[2].isEdit = false;
        this.serverDetail[4].isEdit = false;
        this.isEditChassisInfo = false;
        if (status) {
            this.serverDetail[0].detail = this.serverDetail[0].value ? this.serverDetail[0].value : CommonData.isEmpty;
            this.serverDetail[2].detail = this.serverDetail[2].value ? this.serverDetail[2].value : CommonData.isEmpty;
            this.serverDetail[4].detail = this.serverDetail[4].value !== null ? this.serverDetail[4].value : CommonData.isEmpty;
        } else {
            this.nameControl.reset();
            this.nameControl.clearValidators();
            this.nameControl.setValidators([ChassisVaild.bytesLength()]);
            this.nameControl.updateValueAndValidity();

            this.nameControl.reset();
            this.locationControl.clearValidators();
            this.locationControl.setValidators([ChassisVaild.bytesLength(), ChassisVaild.noChineseCharacters()]);
            this.locationControl.updateValueAndValidity();

            this.nameControl.reset();
            this.idControl.clearValidators();
            this.idControl.setValidators([TiValidators.required, TiValidators.number, TiValidators.rangeValue(0, 999999)]);
            this.idControl.updateValueAndValidity();
        }
    }

    editConfirm() {
        const isValidError = this.nameControl.errors || this.locationControl.errors || this.idControl.errors;
        if (isValidError) {
            return null;
        } else {
            this.loadingService.state.next(true);
            let params = {};
            if (this.idControl.value * 1 !== this.serverDetail[4].value) {
                params = Object.assign(params, {chassisID: this.idControl.value * 1});
            }
            if (this.locationControl.value !== this.serverDetail[2].value) {
                params = Object.assign(params, {chassisLocation: this.locationControl.value});
            }
            if (this.nameControl.value !== this.serverDetail[0].value) {
                params = Object.assign(params, {chassisName: this.nameControl.value});
            }
            this.service.updateChassisInfo(params).then(res => {
                if (res.body) {
                    this.serverDetail[4].value = res.body.ChassisID;
                    this.serverDetail[2].value = res.body.ChassisLocation;
                    this.serverDetail[0].value = res.body.ChassisName;
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    this.editCancel(true);
                }
                this.loadingService.state.next(false);
            })
            .catch(error => {
                this.editCancel();
            });
        }
    }
}
