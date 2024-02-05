import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { BatteryRoutingModule } from './battery-routing.module';
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
    TiSliderModule,
    TiTableModule,
    TiDateModule,
    TiAutocompleteModule,
    TiIconModule
} from '@cloud/tiny3';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { TpLabeleditorModule } from '@cloud/tinyplus3';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { BatteryComponent } from './battery.component';

@NgModule({
    declarations: [BatteryComponent],
    imports: [
        CommonModule,
        BatteryRoutingModule,
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
        TpLabeleditorModule,
        TiTableModule,
        TiDateModule,
        TiAutocompleteModule,
        TiIconModule,
        CommonsModule
    ]
})
export class BatteryModule { }
