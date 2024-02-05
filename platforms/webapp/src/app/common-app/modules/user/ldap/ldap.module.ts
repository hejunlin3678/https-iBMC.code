import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { LdapRoutingModule } from './ldap-routing.module';
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
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { TpHalfmodalModule, TpHelptipModule } from '@cloud/tinyplus3';
import { TranslateModule } from '@ngx-translate/core';
import { LdapService } from './ldap.service';
import { LdapComponent } from './ldap.component';
import { CertifyUploadComponent } from './components/certify-upload/certify-upload.component';
import { HalfModalLdapComponent } from './components/half-modal-ldap/half-modal-ldap.component';
import { LdapCertifyComponent } from './components/ldap-certify/ldap-certify.component';

@NgModule({
    declarations: [
        LdapComponent,
        CertifyUploadComponent,
        HalfModalLdapComponent,
        LdapCertifyComponent
    ],
    imports: [
        CommonModule,
        LdapRoutingModule,
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
    providers: [LdapService]
})
export class LdapModule { }
