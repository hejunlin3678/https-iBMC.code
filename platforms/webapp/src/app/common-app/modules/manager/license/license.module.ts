import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { LicenseRoutingModule } from './license-routing.module';
import {
    TiLeftmenuModule,
    TiSelectModule,
    TiFormfieldModule,
    TiUploadModule,
    TiModalModule,
    TiButtonModule,
    TiTableModule,
    TiSwitchModule,
    TiAlertModule,
    TiProgressbarModule,
    TiValidationModule,
    TiRadioModule, TiIpModule,
    TiCheckgroupModule,
    TiCheckboxModule,
    TiOverflowModule,
    TiTipModule,
    TiTabModule,
    TiDateModule,
    TiPaginationModule
} from '@cloud/tiny3';
import { HttpClientModule } from '@angular/common/http';
import { TranslateModule } from '@ngx-translate/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { TpLabeleditorModule } from '@cloud/tinyplus3';
import { LicenseComponent } from './license.component';
import { CommonsModule } from 'src/app/common-app/components/commons.module';


@NgModule({
    declarations: [LicenseComponent],
    imports: [
        CommonModule,
        LicenseRoutingModule,
        TiLeftmenuModule,
        HttpClientModule,
        TranslateModule,
        TiSelectModule,
        TiFormfieldModule,
        TiUploadModule,
        TiButtonModule,
        TiModalModule,
        TiTableModule,
        FormsModule,
        ReactiveFormsModule,
        TiSwitchModule,
        TiAlertModule,
        TiProgressbarModule,
        TpLabeleditorModule,
        TiValidationModule,
        TiRadioModule,
        TiIpModule,
        TiCheckgroupModule,
        TiCheckboxModule,
        TiTipModule,
        TiOverflowModule,
        TiTabModule,
        TiDateModule,
        TiPaginationModule,
        CommonsModule
    ]
})
export class LicenseModule { }
