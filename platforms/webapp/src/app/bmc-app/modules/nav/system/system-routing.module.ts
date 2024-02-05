import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';


import { SystemComponent } from './system.component';
import { FansGuard, PartityGuard, InfoGuard, MonitorGuard } from 'src/app/common-app/guard';

export const routes: Routes = [
	{
		path: '',
		component: SystemComponent,
		children: [
			{
				path: 'monitor',
				loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.MonitorModule),
				canActivate: [MonitorGuard]
			},
			{
				path: 'info',
				loadChildren: () => import('./info/info.module').then((m) => m.InfoModule),
				canActivate: [InfoGuard]
			},
			{
				path: 'bios',
				loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.BiosModule)
			},
			{
				path: 'storage',
				loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.StorageModule)
			},
			{
				path: 'board',
				loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.BoardModule)
			},
			{
				path: 'power',
                loadChildren: () => import('./power/power.module').then((m) => m.PowerModule),
                canActivate: [InfoGuard]
			},
			{
				path: 'fans',
				loadChildren: () => import('./fans/fans.module').then((m) => m.FansModule),
				canActivate: [FansGuard]
			},
			{
				path: 'partity',
				loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.HardPartityModule),
				canActivate: [PartityGuard]
			},
			{
				path: '',
				pathMatch: 'full',
				redirectTo: 'info'
			}
		]
	}
];

@NgModule({
	imports: [RouterModule.forChild(routes)],
	exports: [RouterModule]
})
export class SystemRoutingModule { }
