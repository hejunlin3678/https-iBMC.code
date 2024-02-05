import { CommonModule } from '@angular/common';
import { NgModule } from '@angular/core';
import { ServiceRoutingModule } from './service-routing.module';
import { ServiceComponent } from './service.component';
import { LeftMenuModule } from 'src/app/common-app/components/left-menu/left-menu.module';

@NgModule({
	declarations: [ServiceComponent],
	imports: [
		CommonModule,
		ServiceRoutingModule,
        LeftMenuModule
	]
})
export class ServiceModule { }
