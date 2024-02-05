import { NgModule } from '@angular/core';
import { NTPComponent } from './ntp.component';
import { NTPRoutingModule } from './ntp-routing.module';
import { TimeAreaComponent } from './time-area/time-area.component';
import { NTPFunctionComponent } from './ntp-function/ntp-function.component';
import { PollingComponent } from './polling/polling.component';
import {
    TiSelectModule,
    TiSwitchModule,
    TiRadioModule,
    TiTextModule,
    TiButtonModule,
    TiValidationModule,
    TiFormfieldModule,
    TiUploadModule,
    TiIconModule,
    TiDatetimeModule
} from '@cloud/tiny3';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';
import { TranslateModule } from '@ngx-translate/core';
import { NtpAuthenComponent } from './ntp-authen/ntp-authen.component';
import { DirectiveModule } from 'src/app/common-app/directives/directive.module';
import { CommonsModule } from 'src/app/common-app/components/commons.module';

@NgModule({
    declarations: [
        NTPComponent,
        TimeAreaComponent,
        NTPFunctionComponent,
        PollingComponent,
        NtpAuthenComponent
    ],
    imports: [
        NTPRoutingModule,
        TiValidationModule,
        TiSelectModule,
        TiRadioModule,
        TiSwitchModule,
        TiTextModule,
        TiButtonModule,
        TiFormfieldModule,
        TiUploadModule,
        TiIconModule,
        TiDatetimeModule,
        FormsModule,
        ReactiveFormsModule,
        CommonModule,
        DirectiveModule,
        CommonsModule,
        TranslateModule
    ]
})
export class NTPModule { }
