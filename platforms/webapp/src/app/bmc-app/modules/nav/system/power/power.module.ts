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
    TiOverflowModule,
    TiSelectModule
} from '@cloud/tiny3';
import { TranslateModule } from '@ngx-translate/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { TpLabeleditorModule } from '@cloud/tinyplus3';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { PowerComponent } from './power.component';
import {} from './power.component';
import {
    PowerInfoComponent,
    PowerCappingComponent,
    PowerControlComponent,
    PowerInfoCardComponent,
    PowerInfoSettingComponent,
    PowerCappingInfoComponent,
    PowerHistoryComponent,
    PowerCappingSettingComponent
} from './component';
import _echarts from 'src/app/common-app/utils/custom-echarts';
import { NgxEchartsModule } from 'ngx-echarts';

@NgModule({
    declarations: [
        PowerComponent,
        PowerInfoComponent,
        PowerCappingComponent,
        PowerControlComponent,
        PowerInfoCardComponent,
        PowerInfoSettingComponent,
        PowerCappingInfoComponent,
        PowerHistoryComponent,
        PowerCappingSettingComponent
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
        TiSelectModule,
        NgxEchartsModule.forRoot({ echarts: Promise.resolve(_echarts) }),
        TiDatetimeRangeModule,
        TpLabeleditorModule,
        TiTextModule,
        TiOverflowModule,
        CommonsModule
    ]
})
export class PowerModule { }
