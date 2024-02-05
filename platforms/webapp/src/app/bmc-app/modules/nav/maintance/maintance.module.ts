import { CommonModule } from '@angular/common';
import { NgModule } from '@angular/core';
import { MaintanceRoutingModule } from './maintance-routing.module';
import { MaintanceComponent } from './maintance.component';
import { LeftMenuModule } from 'src/app/common-app/components/left-menu/left-menu.module';

@NgModule({
	declarations: [
		MaintanceComponent
	],
	imports: [
		CommonModule,
		MaintanceRoutingModule,
        LeftMenuModule
	]
})
export class MaintanceModule { }
