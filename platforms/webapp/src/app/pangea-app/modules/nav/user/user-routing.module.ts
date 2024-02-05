import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { UserComponent } from './user.component';

const routes: Routes = [
	{
		path: '',
		component: UserComponent,
		children: [
			{
				path: 'localuser',
				loadChildren: () => import('src/app/common-app/modules/user').then((m) => m.LocaluserModule)
			},
			{
				path: 'online-user',
				loadChildren: () => import('src/app/common-app/modules/user').then((m) => m.OnlineUserModule)
			},
			{
				path: 'security',
				loadChildren: () => import('src/app/common-app/modules/user').then((m) => m.SecurityModule)
			},
			{
				path: '',
				pathMatch: 'full',
				redirectTo: 'localuser'
			}
		]
	}
];

@NgModule({
	imports: [RouterModule.forChild(routes)],
	exports: [RouterModule]
})
export class UserRoutingModule { }
