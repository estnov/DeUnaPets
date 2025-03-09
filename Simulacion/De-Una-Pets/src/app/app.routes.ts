import { Routes } from '@angular/router';
import { HomePageComponent } from './pages/home-page/home-page.component';
import { CreatePetComponent } from './pages/create-pet/create-pet.component';
import { FirstLoadingComponent } from './pages/first-loading/first-loading.component';
import { InstructionsComponent } from './pages/instructions/instructions.component';
import { HomePage2Component } from './pages/home-page2/home-page2.component';

export const routes: Routes = [
    { path: '', redirectTo: 'loading', pathMatch: 'full' },
    { path: 'home', component: HomePageComponent },
    { path: 'home2', component: HomePage2Component },
    { path: 'create-pet', component: CreatePetComponent},
    { path: 'loading', component: FirstLoadingComponent},
    { path: 'instrucciones', component: InstructionsComponent }, 
    
];
