import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { BmcNetWorkRoutingModule } from './bmc-net-work-routing.module';
import { TranslateModule } from '@ngx-translate/core';
import { NetworkComponent } from './component/network.component';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import {
    TiButtonModule,
    TiFormfieldModule,
    TiIconModule,
    TiIpModule,
    TiModalModule,
    TiOverflowModule,
    TiSwitchModule,
    TiTableModule,
    TiTabModule,
    TiTextModule,
    TiTipModule,
    TiValidationModule
} from '@cloud/tiny3';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { Ipv4NetComponent } from './component/components/ipv4-net/ipv4-net.component';
import { Ipv6NetComponent } from './component/components/ipv6-net/ipv6-net.component';
import { Ipv4BatchSettingComponent } from './component/components/ipv4-batch-setting/ipv4-batch-setting.component';
import { Ipv6BatchSettingComponent } from './component/components/ipv6-batch-setting/ipv6-batch-setting.component';
import { DhcpConfigComponent } from './component/components/dhcp-config/dhcp-config.component';


@NgModule({
    declarations: [
        NetworkComponent,
        Ipv4NetComponent,
        Ipv6NetComponent,
        DhcpConfigComponent,
        Ipv4BatchSettingComponent,
        Ipv6BatchSettingComponent
    ],
    imports: [
        CommonModule,
        CommonsModule,
        FormsModule,
        ReactiveFormsModule,
        TranslateModule,
        TiTabModule,
        TiTextModule,
        TiTableModule,
        TiModalModule,
        TiTipModule,
        TiIconModule,
        TiOverflowModule,
        TiFormfieldModule,
        TiIpModule,
        TiButtonModule,
        TiSwitchModule,
        TiValidationModule,
        BmcNetWorkRoutingModule
    ]
})
export class BmcNetWorkModule { }
