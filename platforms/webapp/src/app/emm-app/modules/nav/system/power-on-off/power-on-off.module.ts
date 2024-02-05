import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { PowerOnOffRoutingModule } from './power-on-off-routing.module';
import { PowerOnOffComponent } from './power-on-off.component';
import { TranslateModule } from '@ngx-translate/core';
import {
    TiValidationModule,
    TiTableModule,
    TiTabModule,
    TiButtonModule,
    TiCheckgroupModule,
} from '@cloud/tiny3';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';
import { CalculateNodeComponent } from './components/calculate-node/calculate-node.component';
import { SwitchModulComponent } from './components/switch-modul/switch-modul.component';


@NgModule({
    declarations: [PowerOnOffComponent, CalculateNodeComponent, SwitchModulComponent],
    imports: [
        CommonModule,
        PowerOnOffRoutingModule,
        TranslateModule,
        TiValidationModule,
        TiTableModule,
        TiButtonModule,
        TiTabModule,
        TiCheckgroupModule,
        PipeModule,
        CommonsModule
    ]
})
export class PowerOnOffModule { }
