import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { AlarmEventRoutingModule } from './alarm-event-routing.module';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { HeaderModule } from 'src/app/common-app/components/header/header.module';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';
import { LeftMenuModule } from 'src/app/common-app/components/left-menu/left-menu.module';
import {
    TiLeftmenuModule,
    TiButtonModule,
    TiTabModule,
    TiSwitchModule,
    TiTableModule,
    TiButtongroupModule,
    TiOverflowModule,
    TiPaginationModule,
    TiCollapseModule,
    TiTextModule,
    TiTipModule,
    TiDateRangeModule,
    TiModalModule,
    TiMessageModule,
    TiAlertModule,
    TiFormfieldModule,
    TiIconModule,
    TiProgressbarModule,
    TiValidationModule
} from '@cloud/tiny3';
import { TpCollapsebuttonModule, TpIconModule, TpHalfmodalModule, TpHelptipModule } from '@cloud/tinyplus3';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { HttpClientModule } from '@angular/common/http';
import { TranslateModule } from '@ngx-translate/core';
import { AlarmEventComponent } from './alarm-event.component';
import { EventTableComponent, FilterConditionComponent } from './component';


@NgModule({
    declarations: [
        AlarmEventComponent,
        EventTableComponent,
        FilterConditionComponent
    ],
    imports: [
        CommonModule,
        AlarmEventRoutingModule,
        CommonsModule,
        HeaderModule,
        PipeModule,
        LeftMenuModule,
        TiLeftmenuModule,
        TiButtonModule,
        TiTabModule,
        TiSwitchModule,
        TiTableModule,
        TiButtongroupModule,
        TiOverflowModule,
        TiPaginationModule,
        TiCollapseModule,
        TiTextModule,
        TiTipModule,
        TiDateRangeModule,
        TiModalModule,
        TiMessageModule,
        TiAlertModule,
        TiIconModule,
        TiFormfieldModule,
        TiProgressbarModule,
        TiValidationModule,
        TpCollapsebuttonModule,
        TpIconModule,
        TpHalfmodalModule,
        TpHelptipModule,
        FormsModule,
        ReactiveFormsModule,
        HttpClientModule,
        TranslateModule
    ]
})
export class AlarmEventModule { }
