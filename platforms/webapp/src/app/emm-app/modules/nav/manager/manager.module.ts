import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { ManagerRoutingModule } from './manager-routing.module';
import { ManagerComponent } from './manager.component';
import { LeftMenuModule } from 'src/app/common-app/components/left-menu/left-menu.module';

@NgModule({
	declarations: [
		ManagerComponent
	],
	imports: [
		CommonModule,
        ManagerRoutingModule,
        LeftMenuModule
	]
})
export class ManagerModule { }
