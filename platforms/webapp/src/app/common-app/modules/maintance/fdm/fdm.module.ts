import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { FdmRoutingModule } from './fdm-routing.module';
import { TranslateModule } from '@ngx-translate/core';
import { FdmComponent } from './fdm.component';
import { DetailsComponent } from './modules/details/details.component';
import { DeviceCaseComponent } from './modules/device-case/device-case.component';
import { TiTreeModule, TiTableModule, TiPaginationModule, TiSelectModule, TiOverflowModule, TiDateRangeModule, TiFormfieldModule} from '@cloud/tiny3';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';

@NgModule({
    declarations: [
        FdmComponent,
        DetailsComponent,
        DeviceCaseComponent
    ],
    imports: [
        CommonModule,
        FormsModule,
        ReactiveFormsModule,
        PipeModule,
        FdmRoutingModule,
        TiTreeModule,
        TiTableModule,
        TiPaginationModule,
        TiSelectModule,
        TiOverflowModule,
        TiDateRangeModule,
        TiFormfieldModule,
        TranslateModule
    ]
})
export class FdmModule { }
