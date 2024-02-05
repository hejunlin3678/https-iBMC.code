import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { UpgradeRoutingModule } from './upgrade-routing.module';

import {
    TiLeftmenuModule,
    TiSelectModule,
    TiFormfieldModule,
    TiUploadModule,
    TiModalModule,
    TiButtonModule,
    TiTableModule,
    TiSwitchModule,
    TiAlertModule,
    TiProgressbarModule,
    TiValidationModule,
    TiRadioModule, TiIpModule,
    TiCheckgroupModule,
    TiCheckboxModule,
    TiOverflowModule,
    TiTipModule,
    TiTabModule,
    TiDateModule,
    TiAccordionModule
} from '@cloud/tiny3';
import { HttpClientModule } from '@angular/common/http';
import { TranslateModule } from '@ngx-translate/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { TpLabeleditorModule } from '@cloud/tinyplus3';
import { UpgradeComponent } from './upgrade.component';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';
import { FanInfoComponent } from './component/fan-info/fan-info.component';
import { LCDComponent } from './component/lcd/lcd.component';
import { EmmInfoComponent } from './component/emm-info/emm-info.component';

@NgModule({
    declarations: [UpgradeComponent, FanInfoComponent, LCDComponent, EmmInfoComponent],
    imports: [
        CommonModule,
        UpgradeRoutingModule,
        TiLeftmenuModule,
        HttpClientModule,
        TranslateModule,
        TiSelectModule,
        TiFormfieldModule,
        TiUploadModule,
        TiButtonModule,
        TiModalModule,
        TiTableModule,
        FormsModule,
        ReactiveFormsModule,
        TiSwitchModule,
        TiAlertModule,
        TiProgressbarModule,
        TpLabeleditorModule,
        TiValidationModule,
        TiRadioModule,
        TiIpModule,
        TiCheckgroupModule,
        TiCheckboxModule,
        TiTipModule,
        TiOverflowModule,
        TiTabModule,
        TiDateModule,
        TiAccordionModule,
        PipeModule,
        CommonsModule
    ]
})
export class UpgradeModule { }
