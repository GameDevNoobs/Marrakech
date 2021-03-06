#include "Game.h"

Game *	Game::m_game = nullptr;

void Game::Render_Callback(ESContext *esContext)
{
	m_game->Render(esContext);
}

void Game::Update_Callback(ESContext *esContext, float deltaTime)
{
	m_game->Update(esContext, deltaTime);
}

void Game::OnKey_Callback(ESContext *esContext, unsigned char key, bool bIsPressed)
{
	m_game->OnKey(esContext, key, bIsPressed);
}

void Game::OnMousePress_callback(ESContext *esContext)
{
	m_game->OnMousePressed();
}

void Game::OnMouseRelease_callback(ESContext *esContext)
{
	m_game->OnMouseReleased();
}

void Game::SetState(GameState state)
{
	if (m_state != state)
		m_state = state;
}

void Game::PushState(const char * key)
{
	if (strcmp("Match", key)==0)
	{
		auto result = m_stateStack.emplace(key, m_match);
		PASSERT(result.second, "State Match is already in stack!");
		return;
	}
	return;
}

void Game::PopState()
{
	m_stateStack.erase(std::prev(m_stateStack.end()));
}

int Game::Init(ESContext *esContext, Game * game)
{
	m_game = game;

	esRegisterDrawFunc(esContext, Render_Callback);
	esRegisterUpdateFunc(esContext, Update_Callback);
	esRegisterKeyFunc(esContext, OnKey_Callback);

	return 0;
}

int Game::Startup(ESContext *esContext)
{
	SetState(LOAD);

	MeshLoader::GetInstance()->LoadMeshes(RESOURCE_PATH);
	TextureLoader::GetInstance()->LoadTextures(RESOURCE_PATH);
	ShaderLoader::GetInstance()->LoadShaders(RESOURCE_PATH);

	m_match = std::make_shared<StateMatch>();
	m_match->Init();
	PushState("Match");

	SetState(IDLE);
	return 0;
}

void Game::Shutdown()
{

}

void Game::Render(ESContext *esContext)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_state != LOAD)
	{
		GetCurrentState()->Render();
	}

	eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
}


void Game::OnKey(ESContext *esContext, unsigned char key, bool bIsPressed)
{
	InputManager::GetInstance()->ManageKeyboard(esContext, key, bIsPressed, m_match->GetScene()->GetCamera().get());
//	InputManager::GetInstance()->ManageCursor(esContext);
}

void Game::OnMousePressed()
{

}

void Game::OnMouseReleased()
{

}

void Game::Update(ESContext *esContext, float deltaTime)
{
	TimeManager::GetInstance()->Update(deltaTime);
}

//////////////////////////////////////////// WINDOWS main function ////////////////////////////////////////////////////
#ifdef _WIN32

#include <conio.h>

int main(int argc, char* argv[])
{
	ESContext esContext;

	esInitContext(&esContext);

	esCreateWindow(&esContext, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, ES_WINDOW_RGB | ES_WINDOW_DEPTH);

	Game * game = new Game();

	if (game->Startup(&esContext) != 0)
		return 0;

	game->Init(&esContext, game);

	esMainLoop(&esContext);

	//releasing OpenGL resources
	game->Shutdown();
	delete game;

	printf("Press any key...\n");
	_getch();

	return 0;
}

#endif
//////////////////////////////////////////// WINDOWS main function ////////////////////////////////////////////////////