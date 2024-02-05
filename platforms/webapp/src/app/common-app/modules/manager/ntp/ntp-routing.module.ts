import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { NTPComponent } from './ntp.component';

const routes: Routes = [
	{
		path: '',
		component: NTPComponent
	}
];

@NgModule({
	imports: [RouterModule.forChild(routes)],
	exports: [RouterModule]
})
export class NTPRoutingModule { }
