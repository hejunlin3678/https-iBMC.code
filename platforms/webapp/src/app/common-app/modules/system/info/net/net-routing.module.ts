import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { NetComponent } from './net.component';
import { InfoGuard } from 'src/app/common-app/guard';

const routes: Routes = [
	{
		path: '',
		component: NetComponent,
		canActivate: [InfoGuard]
	}
];

@NgModule({
	imports: [RouterModule.forChild(routes)],
	exports: [RouterModule]
})
export class NetRoutingModule { }
