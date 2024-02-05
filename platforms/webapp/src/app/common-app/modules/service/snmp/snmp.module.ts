import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { SnmpRoutingModule } from './snmp-routing.module';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import {
    TiLeftmenuModule,
    TiValidationModule,
    TiSelectModule,
    TiFormfieldModule,
    TiTextModule,
    TiTipModule,
    TiAlertModule,
    TiInputNumberModule,
    TiSwitchModule,
    TiButtonModule,
    TiIconModule,
    TiTableModule,
    TiRadioModule,
    TiMessageModule,
    TiModalModule,
    TiUploadModule,
    TiCheckboxModule,
    TiCheckgroupModule,
    TiCollapseModule
} from '@cloud/tiny3';
import { HttpClientModule } from '@angular/common/http';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { LeftMenuModule } from 'src/app/common-app/components/left-menu/left-menu.module';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';
import { TranslateModule } from '@ngx-translate/core';
import { SnmpComponent } from './snmp.component';


@NgModule({
    declarations: [SnmpComponent],
    imports: [
        CommonModule,
        SnmpRoutingModule,
        CommonsModule,
        TiLeftmenuModule,
        TiValidationModule,
        HttpClientModule,
        FormsModule,
        ReactiveFormsModule,
        TiSelectModule,
        TiFormfieldModule,
        TiTextModule,
        TiTipModule,
        TiAlertModule,
        TiInputNumberModule,
        TiSwitchModule,
        TiButtonModule,
        TiIconModule,
        TiTableModule,
        LeftMenuModule,
        TiRadioModule,
        TiMessageModule,
        TiModalModule,
        TiUploadModule,
        TiCheckboxModule,
        TiCheckgroupModule,
        TiCollapseModule,
        PipeModule,
        TranslateModule
    ]
})
export class SnmpModule { }
