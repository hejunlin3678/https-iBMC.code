import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { SystemComponent } from './system.component';

export const routes: Routes = [
	{
		path: '',
		component: SystemComponent,
		children: [
			{
				path: 'power',
				loadChildren: () => import('./power/power.module').then((m) => m.PowerModule)
			},
			{
				path: 'bmcUser',
				loadChildren: () => import('./bmc-user/bmc-user.module').then((m) => m.BmcUserModule)
			},
			{
				path: 'bmcNetwork',
				loadChildren: () => import('./bmc-net-work/bmc-net-work.module').then((m) => m.BmcNetWorkModule)
			},
			{
				path: 'powerOnOff',
				loadChildren: () => import('./power-on-off/power-on-off.module').then((m) => m.PowerOnOffModule)
			},
			{
				path: 'restSwi',
				loadChildren: () => import('./rest-swi/rest-swi.module').then((m) => m.RestSwiModule)
			},
			{
                path: 'fans',
                loadChildren: () => import('./fans/fans.module').then((m) => m.FansModule)
            },
            {
				path: '',
				pathMatch: 'full',
				redirectTo: 'power'
			}
		]
	}
];

@NgModule({
	imports: [RouterModule.forChild(routes)],
	exports: [RouterModule]
})
export class SystemRoutingModule { }
