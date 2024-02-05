import { CommonModule } from '@angular/common';
import { NgModule } from '@angular/core';
import { SystemRoutingModule } from './system-routing.module';
import { SystemComponent } from './system.component';
import { HttpService } from 'src/app/common-app/service';
import { LeftMenuModule } from 'src/app/common-app/components/left-menu/left-menu.module';

@NgModule({
    declarations: [
        SystemComponent
    ],
    imports: [
        CommonModule,
        SystemRoutingModule,
        LeftMenuModule
    ],
    providers: [HttpService]
})
export class SystemModule { }
