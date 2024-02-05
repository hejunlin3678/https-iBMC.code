import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';

import { ReportRoutingModule } from './report-routing.module';
import { SyslogComponent } from './syslog/syslog.component';
import { ReportComponent } from './report.component';
import { EmailComponent } from './email/email.component';
import { TrapComponent } from './trap/trap.component';
import {
    TiTabModule,
    TiFormfieldModule,
    TiSwitchModule,
    TiTextModule,
    TiValidationModule,
    TiRadioModule,
    TiSelectModule,
    TiCheckgroupModule,
    TiCheckboxModule,
    TiButtonModule,
    TiUploadModule,
    TiTipModule,
    TiModalModule,
    TiTableModule,
    TiOverflowModule,
    TiAlertModule,
    TiIconModule
} from '@cloud/tiny3';
import { TableComponent } from './syslog/component/table';
import { CertificateComponent } from './component';
import { TranslateModule } from '@ngx-translate/core';
import { EmailTableComponent } from './email/component';
import { TrapTableComponent } from './trap/component';
import { CommonsModule } from 'src/app/common-app/components/commons.module';

@NgModule({
    declarations: [
        SyslogComponent,
        ReportComponent,
        EmailComponent,
        TrapComponent,
        TableComponent,
        CertificateComponent,
        TrapTableComponent,
        EmailTableComponent
    ],
    imports: [
        CommonModule,
        CommonsModule,
        FormsModule,
        ReactiveFormsModule,
        TiTabModule,
        TiTextModule,
        TiRadioModule,
        TiSelectModule,
        TiTableModule,
        TiButtonModule,
        TiCheckboxModule,
        TiCheckgroupModule,
        TiValidationModule,
        TiUploadModule,
        TiTipModule,
        TiOverflowModule,
        TiAlertModule,
        TiIconModule,
        TiModalModule,
        ReportRoutingModule,
        TiFormfieldModule,
        TiSwitchModule,
        TranslateModule,
    ],
    exports: [
        CertificateComponent
    ]
})
export class ReportModule { }
