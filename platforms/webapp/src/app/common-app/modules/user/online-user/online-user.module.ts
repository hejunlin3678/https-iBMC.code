import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { OnlineUserRoutingModule } from './online-user-routing.module';
import {
    TiValidationModule,
    TiButtonModule,
    TiTableModule,
    TiOverflowModule,
    TiFormfieldModule,
    TiSelectModule,
    TiSwitchModule,
    TiCheckboxModule,
    TiIconModule,
    TiTipModule,
    TiRadioModule,
    TiMessageModule,
    TiTabModule,
    TiModalModule,
    TiCheckgroupModule,
    TiUploadModule,
    TiAlertModule,
    TiSearchboxModule,
    TiTextareaModule,
    TiCollapseModule
} from '@cloud/tiny3';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { TpHalfmodalModule, TpHelptipModule } from '@cloud/tinyplus3';
import { TranslateModule } from '@ngx-translate/core';
import { OnlineUserComponent } from './online-user.component';

@NgModule({
    declarations: [OnlineUserComponent],
    imports: [
        CommonModule,
        OnlineUserRoutingModule,
        FormsModule,
        ReactiveFormsModule,
        CommonsModule,
        TiValidationModule,
        TiButtonModule,
        TiTableModule,
        TiOverflowModule,
        TpHalfmodalModule,
        TpHelptipModule,
        TiFormfieldModule,
        TiSelectModule,
        TiSwitchModule,
        TiCheckboxModule,
        TiIconModule,
        TiTipModule,
        TiSwitchModule,
        TiRadioModule,
        TiMessageModule,
        TiValidationModule,
        TiTabModule,
        TiModalModule,
        TiCheckgroupModule,
        TiUploadModule,
        TiAlertModule,
        TiSearchboxModule,
        TiTextareaModule,
        TiCollapseModule,
        TranslateModule
    ]
})
export class OnlineUserModule { }
