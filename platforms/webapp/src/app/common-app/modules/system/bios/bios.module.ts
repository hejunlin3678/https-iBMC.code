import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { BiosRoutingModule } from './bios-routing.module';
import { TranslateModule } from '@ngx-translate/core';
import {
    TiLeftmenuModule,
    TiValidationModule,
    TiTabModule,
    TiModalModule,
    TiFormfieldModule,
    TiRadioModule,
    TiSelectModule,
    TiSwitchModule,
    TiButtonModule,
    TiAlertModule,
    TiTipModule,
    TiSliderModule
} from '@cloud/tiny3';
import { TpCascaderModule } from '@cloud/tinyplus3';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { TpLabeleditorModule } from '@cloud/tinyplus3';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { BiosComponent } from './bios.component';
import { BootDeviceComponent } from './component/boot-device/boot-device.component';
import { CpuComponent } from './component/cpu/cpu.component';


@NgModule({
    declarations: [BiosComponent, BootDeviceComponent, CpuComponent],
    imports: [
        CommonModule,
        BiosRoutingModule,
        TranslateModule,
        TiLeftmenuModule,
        TiValidationModule,
        FormsModule,
        ReactiveFormsModule,
        TiTabModule,
        TiModalModule,
        TiFormfieldModule,
        TiRadioModule,
        TiSelectModule,
        TiSwitchModule,
        TiButtonModule,
        TiAlertModule,
        TiTipModule,
        TiSliderModule,
        TpLabeleditorModule,
        CommonsModule,
        TpCascaderModule
    ]
})
export class BiosModule { }
