import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { SwitchRoutingModule } from './switch-routing.module';
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
import { TpLabeleditorModule, TpIconModule } from '@cloud/tinyplus3';
import { MainBackSwitchComponent } from './main-back-switch.component';
import { MainBackSwitchService } from './services/main-back-switch.service';


@NgModule({
    declarations: [MainBackSwitchComponent],
    imports: [
        CommonModule,
        SwitchRoutingModule,
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
        TpIconModule,
        TiValidationModule,
        TiRadioModule,
        TiIpModule,
        TiCheckgroupModule,
        TiCheckboxModule,
        TiTipModule,
        TiOverflowModule,
        TiTabModule,
        TiDateModule,
        TiPaginationModule
    ],
    providers: [MainBackSwitchService],
})
export class SwitchModule { }
