import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { KerberosRoutingModule } from './kerberos-routing.module';
import { KerberosComponent } from './kerberos.component';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import {
    TiValidationModule,
    TiButtonModule,
    TiTableModule,
    TiOverflowModule,
    TiFormfieldModule,
    TiSelectModule,
    TiSwitchModule,
    TiCheckboxModule,
    TiIconModule,
    TiTipModule,
    TiRadioModule,
    TiMessageModule,
    TiTabModule,
    TiModalModule,
    TiCheckgroupModule,
    TiUploadModule,
    TiAlertModule,
    TiSearchboxModule,
    TiTextModule,
    TiTextareaModule,
    TiCollapseModule
} from '@cloud/tiny3';
import { TpHalfmodalModule, TpHelptipModule } from '@cloud/tinyplus3';
import { TranslateModule } from '@ngx-translate/core';
import { KerberosService } from './kerberos.service';
import { HalfModalKerberosComponent } from './components/half-modal-kerberos/half-modal-kerberos.component';


@NgModule({
    declarations: [KerberosComponent, HalfModalKerberosComponent],
    imports: [
        KerberosRoutingModule,
        CommonModule,
        FormsModule,
        ReactiveFormsModule,
        CommonsModule,
        TiValidationModule,
        TiButtonModule,
        TiTableModule,
        TiOverflowModule,
        TpHalfmodalModule,
        TpHelptipModule,
        TiFormfieldModule,
        TiSelectModule,
        TiSwitchModule,
        TiCheckboxModule,
        TiIconModule,
        TiTipModule,
        TiSwitchModule,
        TiRadioModule,
        TiMessageModule,
        TiValidationModule,
        TiTabModule,
        TiModalModule,
        TiCheckgroupModule,
        TiUploadModule,
        TiAlertModule,
        TiSearchboxModule,
        TiTextModule,
        TiTextareaModule,
        TiCollapseModule,
        TranslateModule
    ],
    providers: [KerberosService]
})
export class KerberosModule { }
