import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { ProcessorRoutingModule } from './processor-routing.module';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';
import {
    TiTabModule,
    TiTableModule,
    TiFormfieldModule,
    TiValidationModule,
    TiPaginationModule,
    TiSearchboxModule,
    TiTipModule,
    TiOverflowModule,
    TiDateModule,
    TiIconModule
} from '@cloud/tiny3';
import { TpIconModule, TpLabeleditorModule } from '@cloud/tinyplus3';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { TranslateModule } from '@ngx-translate/core';
import { ProcessorComponent } from './processor.component';
import { ProcessorCategoryComponent } from './component';

@NgModule({
    declarations: [ProcessorComponent, ProcessorCategoryComponent],
    imports: [
        CommonModule,
        ProcessorRoutingModule,
        FormsModule,
        ReactiveFormsModule,
        PipeModule,
        TiTabModule,
        TiTableModule,
        TiFormfieldModule,
        TiValidationModule,
        TiPaginationModule,
        TiSearchboxModule,
        TiTipModule,
        TiOverflowModule,
        TpIconModule,
        TiDateModule,
        TiIconModule,
        TpLabeleditorModule,
        CommonsModule,
        TranslateModule,
    ]
})
export class ProcessorModule { }
