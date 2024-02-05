import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { UnitsComponent } from './units.component';
import { UnitsRoutingModule } from './units-routing.module';
import { UnitsNameI18nPipe } from './unitsNameI18n.pipe';
import { TiTableModule, TiFormfieldModule, TiOverflowModule, TiTipModule } from '@cloud/tiny3';
import { TranslateModule } from '@ngx-translate/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { importComponents } from './dynamicComponent';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';


@NgModule({
  declarations: [
    UnitsComponent,
    UnitsNameI18nPipe,
    [...importComponents]
  ],
    imports: [
        CommonModule,
        UnitsRoutingModule,
        PipeModule,
        FormsModule,
        ReactiveFormsModule,
        TiTableModule,
        TiFormfieldModule,
        TiOverflowModule,
        TiTipModule,
        TranslateModule,
    ]
})
export class UnitsModule { }
