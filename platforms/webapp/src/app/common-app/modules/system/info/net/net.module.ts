import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { NetComponent } from './net.component';
import { NetRoutingModule } from './net-routing.module';
import { TranslateModule } from '@ngx-translate/core';
import { NetCardComponent } from './modules/net-card/net-card.component';
import { FcCardComponent } from './modules/fc-card/fc-card.component';
import { BridgeComponent } from './modules/bridge/bridge.component';
import { TeamComponent } from './modules/team/team.component';
import { TiAccordionModule, TiTableModule, TiOverflowModule, TiLeftmenuModule, TiTipModule } from '@cloud/tiny3';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';
import { FcPortComponent, TeamBridgePortComponent } from './modules';
import { NetPortComponent } from './modules/net-port';


@NgModule({
    declarations: [
        NetComponent,
        NetCardComponent,
        NetPortComponent,
        FcCardComponent,
        BridgeComponent,
        TeamComponent,
        FcPortComponent,
        TeamBridgePortComponent
    ],
    imports: [
        CommonModule,
        PipeModule,
        NetRoutingModule,
        TiTableModule,
        TiLeftmenuModule,
        TiAccordionModule,
        TiOverflowModule,
        TiTipModule,
        TranslateModule
    ]
})
export class NetModule { }
