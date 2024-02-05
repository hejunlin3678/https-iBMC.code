import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { PowerRoutingModule } from './power-routing.module';
import {
    TiTabModule,
    TiButtonModule,
    TiFormfieldModule,
    TiModalModule,
    TiRadioModule,
    TiCheckboxModule,
    TiSwitchModule,
    TiTipModule,
    TiButtongroupModule,
    TiValidationModule,
    TiDatetimeRangeModule,
    TiTextModule,
    TiOverflowModule
} from '@cloud/tiny3';
import { TranslateModule } from '@ngx-translate/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { NgxEchartsModule } from 'ngx-echarts';
import { TpLabeleditorModule } from '@cloud/tinyplus3';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { PowerComponent } from './power.component';


@NgModule({
    declarations: [
        PowerComponent
    ],
    imports: [
        CommonModule,
        PowerRoutingModule,
        TranslateModule,
        FormsModule,
        ReactiveFormsModule,
        TiTabModule,
        TiButtonModule,
        TiFormfieldModule,
        TiModalModule,
        TiRadioModule,
        TiCheckboxModule,
        TiSwitchModule,
        TiTipModule,
        TiButtongroupModule,
        TiValidationModule,
        NgxEchartsModule.forRoot({
            echarts: () => import('echarts')
        }),
        TiDatetimeRangeModule,
        TpLabeleditorModule,
        TiTextModule,
        TiOverflowModule,
        CommonsModule
    ]
})
export class PowerModule { }
