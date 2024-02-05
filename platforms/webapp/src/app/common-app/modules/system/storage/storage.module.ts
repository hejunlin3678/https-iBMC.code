import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { StorageRoutingModule } from './storage-routing.module';
import { StorageComponent } from './storage.component';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import {
    TiTreeModule,
    TiModalModule,
    TiSwitchModule,
    TiFormfieldModule,
    TiTextModule,
    TiTipModule,
    TiSelectModule,
    TiRadioModule,
    TiButtonModule,
    TiCheckboxModule,
    TiSliderModule,
    TiAlertModule,
    TiMessageModule,
    TiValidationModule
} from '@cloud/tiny3';
import { importComponents } from './components/dynamicComponent';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';
import { TranslateModule, TranslateLoader } from '@ngx-translate/core';
import { HttpClient } from '@angular/common/http';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { TpHalfmodalModule, TpHelptipModule } from '@cloud/tinyplus3';
import { DirectiveModule } from 'src/app/common-app/directives/directive.module';
import { createTranslateHttpLoader } from 'src/app/app.module';
import { VolumnSPEditComponent } from './components/volumes/volumn-sp-edit/volumn-sp-edit.component';

@NgModule({
    declarations: [
        [...importComponents],
        StorageComponent,
        VolumnSPEditComponent
    ],
    imports: [
        CommonModule,
        FormsModule,
        ReactiveFormsModule,
        StorageRoutingModule,
        CommonsModule,
        TpHalfmodalModule,
        TpHelptipModule,
        TiTreeModule,
        PipeModule,
        TiButtonModule,
        TiModalModule,
        TiSwitchModule,
        TiRadioModule,
        TiFormfieldModule,
        TiTextModule,
        TiTipModule,
        TiSelectModule,
        TiCheckboxModule,
        TiSliderModule,
        TiAlertModule,
        TiMessageModule,
        TiValidationModule,
        DirectiveModule,
        TranslateModule.forChild({
            loader: {
                provide: TranslateLoader,
                useFactory: createTranslateHttpLoader,
                deps: [HttpClient]
            }
        })
    ]
})
export class StorageModule { }
