import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { AssetRoutingModule } from './asset-routing.module';
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
    TiIconModule,
    TiOverflowModule
} from '@cloud/tiny3';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { TpLabeleditorModule } from '@cloud/tinyplus3';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { AssetComponent } from './asset.component';
import { AssetEditComponent } from './component/asset-edit/asset-edit.component';


@NgModule({
    declarations: [AssetComponent, AssetEditComponent],
    imports: [
        CommonModule,
        AssetRoutingModule,
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
        TiOverflowModule,
        CommonsModule
    ]
})
export class AssetModule { }
