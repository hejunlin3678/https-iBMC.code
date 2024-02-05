import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { MonitorRoutingModule } from './monitor-routing.module';
import { TranslateModule } from '@ngx-translate/core';
import {
    TiLeftmenuModule,
    TiValidationModule,
    TiTabModule,
    TiModalModule,
    TiFormfieldModule,
    TiRadioModule,
    TiSelectModule,
    TiSwitchModule,
    TiButtonModule,
    TiAlertModule,
    TiTipModule,
    TiSliderModule
} from '@cloud/tiny3';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { TpLabeleditorModule } from '@cloud/tinyplus3';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { MonitorComponent } from './monitor.component';
import { DiskChartsComponent } from './component/disk-charts/disk-charts.component';
import { MonitorChartsComponent } from './component/monitor-charts/monitor-charts.component';
import { NetworkChartsComponent } from './component/network-charts/network-charts.component';
import _echarts from 'src/app/common-app/utils/custom-echarts';
import { NgxEchartsModule } from 'ngx-echarts';

@NgModule({
    declarations: [
        MonitorComponent,
        DiskChartsComponent,
        MonitorChartsComponent,
        NetworkChartsComponent
    ],
    imports: [
        CommonModule,
        MonitorRoutingModule,
        TranslateModule,
        TiLeftmenuModule,
        TiValidationModule,
        FormsModule,
        ReactiveFormsModule,
        TiTabModule,
        TiModalModule,
        TiFormfieldModule,
        TiRadioModule,
        TiSelectModule,
        TiSwitchModule,
        TiButtonModule,
        TiAlertModule,
        TiTipModule,
        TiSliderModule,
        NgxEchartsModule.forRoot({ echarts: Promise.resolve(_echarts) }),
        TpLabeleditorModule,
        CommonsModule
    ]
})
export class MonitorModule { }
