import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { AlarmConfigRoutingModule } from './alarm-config-routing.module';
import { AlarmConfigComponent } from './alarm-config.component';
import { TranslateModule } from '@ngx-translate/core';
import { CommonsModule } from 'src/app/common-app/components/commons.module';


import {
    TiIconModule,
    TiFormfieldModule,
    TiTipModule,
    TiMessageModule,
    TiAlertModule,
    TiButtonModule,
    TiTableModule,
    TiOverflowModule,
    TiSelectModule,
    TiTextModule
} from '@cloud/tiny3';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';


@NgModule({
    declarations: [
        AlarmConfigComponent
    ],
    imports: [
        CommonModule,
        CommonsModule,
        FormsModule,
        ReactiveFormsModule,
        TranslateModule,
        AlarmConfigRoutingModule,
        TiIconModule,
        TiFormfieldModule,
        TiTipModule,
        TiMessageModule,
        TiAlertModule,
        TiButtonModule,
        TiTableModule,
        TiOverflowModule,
        TiSelectModule,
        TiTextModule
    ]
})
export class AlarmConfigModule { }
