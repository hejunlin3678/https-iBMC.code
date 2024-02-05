import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { UserRoutingModule } from './user-routing.module';
import { UserComponent } from './user.component';
import { LeftMenuModule } from 'src/app/common-app/components/left-menu/left-menu.module';

@NgModule({
	declarations: [
		UserComponent
	],
	imports: [
		CommonModule,
		UserRoutingModule,
        LeftMenuModule
	]
})
export class UserModule { }
