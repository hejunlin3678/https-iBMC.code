import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { AuthGuard, KvmLoginGuard } from '../common-app/guard';
import { BMCAppComponent } from './bmc-app.component';
import { BMCGuard } from './guard/bmc-guard.service';
import { AppGuard } from '../common-app/guard/app-guard.service';

const routes: Routes = [
    {
		path: '',
        component: BMCAppComponent,
        canActivate: [AppGuard, BMCGuard],
        children: [
            {
                path: 'login',
                loadChildren: () => import('./modules/login/login.module').then((m) => m.LoginModule),
                canActivateChild: [KvmLoginGuard]
            },
            {
                path: 'videoPlayer/:id',
                loadChildren: () => import('./modules/virtualControl/video-player/video-player.module').then((m) => m.VideoPlayerModule)
            },
            {
                path: 'kvm_h5',
                loadChildren: () => import('./modules/virtualControl/kvm-h5/kvm-h5.module').then((m) => m.KvmH5Module)
            },
            {
                path: 'navigate',
                loadChildren: () => import('./modules/nav/nav.module').then((m) => m.NavModule),
                canActivateChild: [AuthGuard]
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'login'
            },
            {
                path: '**',
                pathMatch: 'full',
                redirectTo: 'login'
            }
        ]
	}
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class BmcAppRoutingModule { }
