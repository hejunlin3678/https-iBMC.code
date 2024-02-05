import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { TiTableModule, TiFormfieldModule, TiOverflowModule, TiTipModule } from '@cloud/tiny3';
import { OthersRoutingModule } from './others-routing.module';
import { OthersComponent } from './others.component';
import { importComponents } from './dynamicComponent';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';
import { TranslateModule } from '@ngx-translate/core';
import { CardNameI18nPipe } from './cardNameI18n.pipe';



@NgModule({
    declarations: [
        OthersComponent,
        CardNameI18nPipe,
        [...importComponents]
    ],
    entryComponents: [
        [...importComponents],
    ],
    imports: [
        CommonModule,
        FormsModule,
        ReactiveFormsModule,
        OthersRoutingModule,
        PipeModule,
        TiTableModule,
        TiFormfieldModule,
        TiOverflowModule,
        TiTipModule,
        TranslateModule,
    ]
})
export class OthersModule { }
