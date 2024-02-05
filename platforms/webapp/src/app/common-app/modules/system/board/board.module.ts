import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { BoardRoutingModule } from './board-routing.module';
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
import { BoardComponent } from './board.component';


@NgModule({
    declarations: [BoardComponent],
    imports: [
        CommonModule,
        BoardRoutingModule,
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
        CommonsModule
    ]
})
export class BoardModule { }
