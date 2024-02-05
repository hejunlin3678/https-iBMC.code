import { Routes } from '@angular/router';
import { FansGuard } from 'src/app/common-app/guard';
import { SystemComponent } from '../system.component';

export const pacificRoutes: Routes = [
	{
		path: '',
		component: SystemComponent,
		children: [
			{
				path: 'info',
				loadChildren: () => import('../info/info.module').then((m) => m.InfoModule)
			},
			{
				path: 'power',
				loadChildren: () => import('../power/power.module').then((m) => m.PowerModule)
			},
			{
				path: 'fans',
				loadChildren: () => import('../fans/fans.module').then((m) => m.FansModule),
				canActivate: [FansGuard]
			},
			{
				path: '',
				pathMatch: 'full',
				redirectTo: 'info'
			}
		]
	}
];
